#include "netlib_server.h"
#include "netlib_private.h"

#include "nwol_misc.h"

#include "gref_definition.h"
#include "nwol_size.h"

#include <thread>
#if defined(__WINDOWS__)
#include <process.h>
#endif

#include <time.h>
#include <thread>

#if defined(__WINDOWS__)
#include <WinSock2.h>
#endif

GDEFINE_OBJ(nwol, CClient);

int32_t									nwol::CServer::InitServer						(int32_t port_number)														{
	::gethostname(HostName, sizeof(HostName));	// Get host name of this computer
	QueuedConnectionCount								= 0;

	::nwol::error_t											
	errMy												= ::nwol::createConnectionByHostName( HostName, (uint16_t)port_number, &ServerConnection );	reterr_error_if_errored(errMy, "%s", "Failed to initialize connection."					);
	errMy												= ::nwol::initConnection(ServerConnection);													reterr_error_if_errored(errMy, "Failed to initialize server socket. Shutdown: 0x%u."	, ShutdownServer());
	errMy												= ::nwol::bindConnection(ServerConnection);													reterr_error_if_errored(errMy, "Failed to bind server socket. Shutdown: 0x%u."			, ShutdownServer());

	bListening											= true;
	return 0;
}

int32_t									receiveSystemCommand							(::nwol::CClient* pClient, ::nwol::NETLIB_COMMAND& commandReceived)			{
	return receiveSystemCommand(pClient->m_ClientListener, pClient->m_ClientTarget, commandReceived);
}

int32_t									nwol::CServer::Listen							()																			{
	if(!bListening)
		return 1;

	int														a1, a2, a3, a4, port_number;	// Components of address in xxx.xxx.xxx.xxx form
	::nwol::getAddress( ServerConnection, &a1, &a2, &a3, &a4, &port_number );
	info_printf("Server listening on %u.%u.%u.%u:%u"
		, (uint32_t)a1
		, (uint32_t)a2
		, (uint32_t)a3
		, (uint32_t)a4
		, (uint32_t)ntohs((u_short)port_number)
		);
	// -- Receive bytes from client
	SConnectionEndpoint										* client							= nullptr;			// Information about the client
	int32_t													bytes_received						= 0;
	::nwol::NETLIB_COMMAND									command								= NETLIB_COMMAND_INVALID;
	::nwol::error_t											errMy								= ::nwol::receiveFromConnection( ServerConnection, (ubyte_t*)&command, sizeof(NETLIB_COMMAND), &bytes_received, &client );
	reterr_error_if(bytes_received < 0, "Could not receive datagram. 0x%x.", errMy);

	errMy												= ::nwol::getAddress( client, &a1, &a2, &a3, &a4, &port_number );
	info_printf("Received %u bytes from %u.%u.%u.%u:%u. Command: %s", bytes_received
		, (uint32_t)a1
		, (uint32_t)a2
		, (uint32_t)a3
		, (uint32_t)a4
		, (uint32_t)port_number
		, ::nwol::get_value_label(command).c_str()
		);
	int64_t													newIndex							= INTERLOCKED_INCREMENT(QueuedConnectionCount)-1;
	if(bListening && command == ::nwol::NETLIB_COMMAND_CONNECT && newIndex < (int64_t)::nwol::size(QueuedConnectionList))
		QueuedConnectionList[newIndex]						= client;
	else
		::nwol::shutdownConnection(&client);

	return 0;
}

void									disconnectClient								(nwol::CClient* client)														{
	/* Get current time */
	info_printf("Disconnecting client %u.", client->m_id);
	::nwol::shutdownConnection(&client->m_ClientListener);
	::nwol::shutdownConnection(&client->m_ClientTarget);
}

int32_t									sendSystemCommand								(::nwol::CClient* pClient, const ::nwol::NETLIB_COMMAND& commandToSend)		{ return ::nwol::sendSystemCommand(pClient->m_ClientListener, pClient->m_ClientTarget, commandToSend);	}
int32_t									processCommandInternal							(::nwol::CClient* client, ::nwol::NETLIB_COMMAND command)					{
	info_printf("Processing system command: %s.", ::nwol::get_value_label(command).c_str());

	if (command == ::nwol::NETLIB_COMMAND_DISCONNECT)		{
		debug_print("Disconnect requested by client.");
		disconnectClient(client);
	}
	else if (command == ::nwol::NETLIB_COMMAND_PING)
		sendSystemCommand(client, ::nwol::NETLIB_COMMAND_PONG);	// Pong client
	else if (command == ::nwol::NETLIB_COMMAND_TIME_GET)	{
		sendSystemCommand(client, ::nwol::NETLIB_COMMAND_TIME_SET);

		uint64_t									current_time									= time(0);
		int32_t										sentBytes										= 0;
		// Send data back
		::nwol::error_t								errMy											= ::nwol::sendToConnection(client->m_ClientListener, (ubyte_t *)&current_time, (int)sizeof(current_time), &sentBytes, client->m_ClientTarget);
		reterr_error_if(0 > errMy || sentBytes != (int32_t)sizeof(current_time), "Failed to send time to client. 0x%x.", errMy);
		// Display time
		char										timestring		[256]							= {};
		time_t										curTimeWithUnreliableSize						= current_time;
#if defined(__WINDOWS__)
		ctime_s(timestring, sizeof(char)*256, &curTimeWithUnreliableSize);
#else
		strcpy(timestring, ctime(&curTimeWithUnreliableSize));
#endif
		int											port_number;			// Port number to use
		int											a1, a2, a3, a4;			// Components of address in xxx.xxx.xxx.xxx form
		errMy									= ::nwol::getAddress(client->m_ClientTarget, &a1, &a2, &a3, &a4, &port_number);
		timestring[strlen(timestring)-1]		= 0;
		info_printf("Sent time (%s) to %u.%u.%u.%u:%u."
			, timestring
			, (int)a1
			, (int)a2
			, (int)a3
			, (int)a4
			, (int)port_number
		);
	}
	else if (command == ::nwol::NETLIB_COMMAND_USER_REQUEST)	{
		char										buffer			[256]							= {};
		return executeCommand(client, buffer);
	}
	else
		sendSystemCommand(client, ::nwol::NETLIB_COMMAND_INVALID);
	
	return 0;
}

void									clientProc										(void *pvClient)															{
	::nwol::CClient								* pClient										= (::nwol::CClient*)pvClient;

	do {
		// Receive bytes from client
		::nwol::NETLIB_COMMAND					command											= ::nwol::NETLIB_COMMAND_INVALID;
		::nwol::error_t							errMy											= receiveSystemCommand(pClient, command);
		if(0 > errMy)													{
			error_print("Failed to receive command from client.");
			break;
		}
		if(0 > (errMy = processCommandInternal( pClient, command )))	{
			error_printf("processCommandInternal failed for command: '%s' (0x%x)(%u).", ::nwol::get_value_label(command).c_str(), (uint32_t)command, (uint32_t)command);
			break;
		}
    }
    while ( pClient->m_ClientListener && pClient->m_ClientTarget );	// Repeat 

	::std::this_thread::sleep_for(::std::chrono::milliseconds(1000));
	disconnectClient(pClient); // This used to cause a crash if the server is shutting down because of a thread race. 
	// Shutting down the server already calls this function which invalidates the pointers causing a null pointer access.
	// However, the sleep_for call prevented this thread to rush when the server is shutting down and in general allows the pointers to be invalidated on time
	// causing the second call to find detect these null pointers and exit gracefully without error.
	// A better synchronization is desired but I don't think it's worth spending time on it until we find this crashing at least once. In practice it never did after the addition of the sleep_for().
}

int32_t									nwol::CServer::Accept							()																			{
	if( 0 == QueuedConnectionCount )
		return 1;

	static int32_t											connectionsAccepted					= 0;

	int32_t													a1, a2, a3, a4, local_port_number, client_port_number;	// Components of address in xxx.xxx.xxx.xxx form
	::nwol::SConnectionEndpoint								* newClientListener					= 0
		,													* targetConn						= QueuedConnectionList[INTERLOCKED_DECREMENT(QueuedConnectionCount)]
		;

	::nwol::getAddress( ServerConnection, &a1, &a2, &a3, &a4, &local_port_number );
	::nwol::error_t											
	result												= ::nwol::createConnection(a1, a2, a3, a4, 0, &newClientListener);	reterr_error_if_errored(result, "%s", "Failed to create client listener.");
	result												= ::nwol::initConnection(newClientListener);						reterr_error_if_errored(result, "%s", "Failed to initialize client listener connection.");
	result												= ::nwol::bindConnection(newClientListener);						reterr_error_if_errored(result, "%s", "Failed to bind client listener connection.");
	::nwol::getAddress( newClientListener, &a1, &a2, &a3, &a4, &local_port_number );
	
	GPtrObj(CClient)										newClient;
	bool													bFound									= false
		,													bFoundEmpty								= false
		;
	uint32_t												indexFound								= ~0U;
	{
		::nwol::CLock											lock									(ConnectionsMutex);
		for(uint32_t iClient=0, clientCount = ClientConnections.size(); iClient<clientCount; ++iClient) {
			GREF(CClient)											* refClient								= ClientConnections.begin()[iClient];
			if(0 == refClient) {
				bFoundEmpty											= true;
				indexFound											= iClient;
				break;
			}
			else if(refClient->get()->m_ClientListener == 0 || refClient->get()->m_ClientTarget == 0) {
				newClient											= ::nwol::acquire(refClient);
				bFound												= true;
				break;
			}
		}
		if(newClient)
			newClient->InitClient( newClientListener, targetConn, newClient->m_id );
		else
			newClient->InitClient( newClientListener, targetConn, connectionsAccepted++ );

		if(bFoundEmpty)
			ClientConnections.set(newClient, indexFound);
		else if(!bFound) {
			if(-1 == ClientConnections.push_back(newClient)) {
				error_printf("Failed to push client connection. Out of memory?");
				disconnectClient(newClient.get_address());
				return -1;
			}
		}
	}
#if defined(__WINDOWS__)
	_beginthread( clientProc, 0, newClient.get_address() );
#else
#error "Not implemented."
#endif
	// Build listening port message
	result												= ::sendSystemCommand(newClient.get_address(), NETLIB_COMMAND_PORT);
	reterr_error_if_errored(result, "%s", "Failed to send port command to client.");

	int32_t													sentBytes							= 0;
	local_port_number									= htons((u_short)local_port_number);
	result												= ::nwol::sendToConnection( newClientListener, (const ubyte_t*)&local_port_number, sizeof(int32_t), &sentBytes, targetConn );
	reterr_error_if_errored(result, "Failed to send port number to client: %u", (uint32_t)local_port_number);
	reterr_error_if(sentBytes != sizeof(int32_t), "%s", "Failed to send port command to client.");

	// Display time
	::nwol::getAddress( targetConn, &a1, &a2, &a3, &a4, &client_port_number );
	info_printf("Sent available port (%u) to %u.%u.%u.%u:%u." 
		, (uint32_t)local_port_number
		, (uint32_t)a1
		, (uint32_t)a2
		, (uint32_t)a3
		, (uint32_t)a4
		, (uint32_t)client_port_number
		);

	return 0;
};

::nwol::error_t							nwol::CServer::ShutdownServer					()																			{
	::nwol::CLock											lockClients							(ConnectionsMutex);
	bListening											= false;

	while ( QueuedConnectionCount > 0 )
		::nwol::shutdownConnection(&QueuedConnectionList[INTERLOCKED_DECREMENT(QueuedConnectionCount)]);

	while ( ClientConnections.size() ) {
		GPtrNCO(CClient)										client;
		ClientConnections.pop(&client);
		if(0 == client)
			continue;

		// Close sockets for this client.
		disconnectClient(client.get_address());
	}
	if( ServerConnection ) {
		::nwol::shutdownConnection(&ServerConnection);
		ServerConnection									= 0;
	}
	return 0;
}

void									nwol::CServer::DisconnectClient					(int32_t id)																{
	::nwol::CLock											lockClients							(ConnectionsMutex);
	for(uint32_t i=0; i<ClientConnections.size(); ++i) {
		CClient													* client							= ClientConnections[i];
		if(client && client->m_id == id) {
			disconnectClient(client);
			NWOH(CClient)											endClient;
			ClientConnections.pop(&endClient);
			ClientConnections.set(endClient, i);
		}
	}
}
