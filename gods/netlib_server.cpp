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
GDEFINE_OBJ(nwol, SClientInputBuffer);

int32_t												nwol::CServer::InitServer						(int32_t port_number)														{
	::gethostname(HostName, sizeof(HostName));	// Get host name of this computer
	QueuedConnectionCount								= 0;
	nwol_necall(::nwol::createConnectionByHostName( HostName, (uint16_t)port_number, &ServerConnection )	, "%s", "Failed to initialize connection."				);
	nwol_necall(::nwol::initConnection(ServerConnection)													, "Failed to initialize server socket. Shutdown: 0x%u."	, ShutdownServer());
	nwol_necall(::nwol::bindConnection(ServerConnection)													, "Failed to bind server socket. Shutdown: 0x%u."		, ShutdownServer());
	bListening											= true;
	return 0;
}

int32_t												receiveSystemCommand							(::nwol::CClient* pClient, ::nwol::NETLIB_COMMAND& commandReceived)			{
	return receiveSystemCommand(pClient->ClientListener, pClient->ClientTarget, commandReceived);
}

int32_t												nwol::CServer::Listen							()																			{
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

void												disconnectClient								(nwol::CClient* client)														{
	// Get current time
	info_printf("Disconnecting client %u.", client->Id);
	::nwol::shutdownConnection(&client->ClientListener);
	::nwol::shutdownConnection(&client->ClientTarget);
}

int32_t												sendSystemCommand								(::nwol::CClient* pClient, const ::nwol::NETLIB_COMMAND& commandToSend)		{ return ::nwol::sendSystemCommand(pClient->ClientListener, pClient->ClientTarget, commandToSend);	}
int32_t												processCommandInternal							(::nwol::CClient* client, ::nwol::NETLIB_COMMAND command)					{
	info_printf("Processing system command: %s.", ::nwol::get_value_label(command).c_str());

//	char										receiveSmallBuf		[256]							= {};
	if (command == ::nwol::NETLIB_COMMAND_DISCONNECT) {
		debug_print("Disconnect requested by client.");
		disconnectClient(client);
	}
	else if (command == ::nwol::NETLIB_COMMAND_PING) {
		nwol_necall(::sendSystemCommand(client, ::nwol::NETLIB_COMMAND_PONG), "Failed to pong client.");	// Pong client
	}
	else if (command == ::nwol::NETLIB_COMMAND_TIME_GET) {
		::sendSystemCommand(client, ::nwol::NETLIB_COMMAND_TIME_SET);

		uint64_t												current_time									= time(0);
		int32_t													sentBytes										= 0;
		// Send data back
		::nwol::error_t											errMy											= ::nwol::sendToConnection(client->ClientListener, (ubyte_t *)&current_time, (int)sizeof(current_time), &sentBytes, client->ClientTarget);
		reterr_error_if(0 > errMy || sentBytes != (int32_t)sizeof(current_time), "Failed to send time to client. 0x%x.", errMy);
		// Display time
		char													timestring		[256]							= {};
		int64_t													curTimeWithUnreliableSize						= current_time;
#if defined(__WINDOWS__)
		ctime_s(timestring, sizeof(char)*256, &curTimeWithUnreliableSize);
#else
		strcpy(timestring, ctime(&curTimeWithUnreliableSize));
#endif
		int														port_number;			// Port number to use
		int														a1, a2, a3, a4;			// Components of address in xxx.xxx.xxx.xxx form
		errMy												= ::nwol::getAddress(client->ClientTarget, &a1, &a2, &a3, &a4, &port_number);
		timestring[strlen(timestring)-1]					= 0;
		info_printf("Sent time (%s) to %u.%u.%u.%u:%u."
			, timestring
			, (int)a1
			, (int)a2
			, (int)a3
			, (int)a4
			, (int)port_number
		);
	}
	else if (command == ::nwol::NETLIB_COMMAND_USER_REQUEST) {
		char													buffer			[256]							= {};
		int32_t													payloadSize										= 0;
		int32_t													receivedBytes									= 0;
		::std::vector<ubyte_t>									payloadBytes;
		nwol_necall(::nwol::receiveFromConnection(client->ClientListener, (ubyte_t*)&payloadSize, sizeof(int32_t), &receivedBytes, 0), "Failed to receive payload size from remote connection.");
		switch(payloadSize) {
		case 1	: { uint8_t  payloadValue = (uint8_t	)~0; nwol_necall(::nwol::receiveFromConnection(client->ClientListener, (ubyte_t*)&payloadValue, sizeof(uint8_t  ), &receivedBytes, 0), "Failed to receive payload size from remote connection."); return executeCommand(client, (const char*)&payloadValue, payloadSize); } 
		case 2	: { uint16_t payloadValue = (uint16_t	)~0; nwol_necall(::nwol::receiveFromConnection(client->ClientListener, (ubyte_t*)&payloadValue, sizeof(uint16_t ), &receivedBytes, 0), "Failed to receive payload size from remote connection."); return executeCommand(client, (const char*)&payloadValue, payloadSize); } 
		case 4	: { uint32_t payloadValue = (uint32_t	)~0; nwol_necall(::nwol::receiveFromConnection(client->ClientListener, (ubyte_t*)&payloadValue, sizeof(uint32_t ), &receivedBytes, 0), "Failed to receive payload size from remote connection."); return executeCommand(client, (const char*)&payloadValue, payloadSize); } 
		case 8	: { uint64_t payloadValue = (uint64_t	)~0; nwol_necall(::nwol::receiveFromConnection(client->ClientListener, (ubyte_t*)&payloadValue, sizeof(uint64_t ), &receivedBytes, 0), "Failed to receive payload size from remote connection."); return executeCommand(client, (const char*)&payloadValue, payloadSize); } 
		default	:
			ree_if(payloadSize > 65535 || payloadSize <= 0, "Invalid payload size: %u", payloadSize);
			payloadBytes.resize(payloadSize);
			nwol_necall(::nwol::receiveFromConnection(client->ClientListener, (ubyte_t*)&payloadBytes[0], payloadSize, &receivedBytes, 0), "Failed to receive payload size from remote connection.");
			return executeCommand(client, buffer, payloadSize);
		}
	}
	else
		nwol_necall(sendSystemCommand(client, ::nwol::NETLIB_COMMAND_INVALID), "Failed to report invalid command to client.");
	
	return 0;
}

void												clientProc										(void *pvClient)															{
	::nwol::CClient											* pClient										= (::nwol::CClient*)pvClient;

	do {
		// Receive bytes from client
		::nwol::NETLIB_COMMAND								command											= ::nwol::NETLIB_COMMAND_INVALID;
		break_error_if(receiveSystemCommand(pClient, command), "Failed to receive command from client.");
		break_error_if(errored(processCommandInternal(pClient, command)), "processCommandInternal failed for command: '%s' (0x%x)(%u).", ::nwol::get_value_label(command).c_str(), (uint32_t)command, (uint32_t)command);
    }
    while ( pClient->ClientListener && pClient->ClientTarget );	// Repeat 

	::std::this_thread::sleep_for(::std::chrono::milliseconds(1000));
	disconnectClient(pClient); // This used to cause a crash if the server is shutting down because of a thread race. 
	// Shutting down the server already calls this function which invalidates the pointers causing a null pointer access.
	// However, the sleep_for call prevented this thread to rush when the server is shutting down and in general allows the pointers to be invalidated on time
	// causing the second call to detect these null pointers and exit gracefully without error.
	// A better synchronization is desired but I don't think it's worth spending time on it until we find this crashing at least once. In practice it never did after the addition of the sleep_for().
}

int32_t												nwol::CServer::Accept							()																			{
	if( 0 == QueuedConnectionCount )
		return 1;

	static int32_t											connectionsAccepted								= 0;

	int32_t													a1, a2, a3, a4, local_port_number, client_port_number;	// Components of address in xxx.xxx.xxx.xxx form
	::nwol::SConnectionEndpoint								* newClientListener								= 0
		,													* targetConn									= QueuedConnectionList[INTERLOCKED_DECREMENT(QueuedConnectionCount)]
		;
	nwol_necall(::nwol::getAddress(ServerConnection, &a1, &a2, &a3, &a4, &local_port_number)	, "Failed to resolve local address.");
	nwol_necall(::nwol::createConnection(a1, a2, a3, a4, 0, &newClientListener)					, "%s", "Failed to create client listener.");
	nwol_necall(::nwol::initConnection(newClientListener)										, "%s", "Failed to initialize client listener connection.");
	nwol_necall(::nwol::bindConnection(newClientListener)										, "%s", "Failed to bind client listener connection.");
	nwol_necall(::nwol::getAddress(newClientListener, &a1, &a2, &a3, &a4, &local_port_number)	, "Failed to resolve remote address.");

	GPtrObj(CClient)										newClient;
	bool													bFound											= false
		,													bFoundEmpty										= false
		;
	uint32_t												indexFound										= ~0U;
	{
		::nwol::CLock											lock											(ConnectionsMutex);
		for(uint32_t iClient=0, clientCount = ClientConnections.size(); iClient<clientCount; ++iClient) {
			GREF(CClient)											* refClient										= ClientConnections.begin()[iClient];
			if(0 == refClient) {
				bFoundEmpty											= true;
				indexFound											= iClient;
				break;
			}
			else if(refClient->get()->ClientListener == 0 || refClient->get()->ClientTarget == 0) {
				newClient											= ::nwol::acquire(refClient);
				bFound												= true;
				break;
			}
		}
		if(newClient)
			newClient->InitClient( newClientListener, targetConn, newClient->Id );
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
	nwol_necall(::sendSystemCommand(newClient.get_address(), NETLIB_COMMAND_PORT), "%s", "Failed to send port command to client.");	// Build listening port message
	int32_t													sentBytes										= 0;
	local_port_number									= htons((u_short)local_port_number);
	nwol_necall(::nwol::sendToConnection( newClientListener, (const ubyte_t*)&local_port_number, sizeof(int32_t), &sentBytes, targetConn ), "Failed to send port number to client: %u", (uint32_t)local_port_number);
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

::nwol::error_t										nwol::CServer::ShutdownServer					()																			{
	::nwol::CLock											lockClients										(ConnectionsMutex);
	bListening											= false;

	while ( QueuedConnectionCount > 0 )
		::nwol::shutdownConnection(&QueuedConnectionList[INTERLOCKED_DECREMENT(QueuedConnectionCount)]);

	while ( ClientConnections.size() ) {
		GPtrNCO(CClient)										client;
		ClientConnections.pop(&client);
		if(client)	// Close sockets for this client.
			disconnectClient(client.get_address());
	}
	if( ServerConnection ) {
		::nwol::shutdownConnection(&ServerConnection);
		ServerConnection									= 0;
	}
	return 0;
}

void												nwol::CServer::DisconnectClient					(int32_t id)																{
	::nwol::CLock											lockClients										(ConnectionsMutex);
	for(uint32_t i = 0; i < ClientConnections.size(); ++i) {
		CClient													* client										= ClientConnections[i];
		if(client && client->Id == id) {
			disconnectClient(client);
			NWOH(CClient)											endClient;
			error_if(0 > ClientConnections.pop(&endClient), "The loop is broken! No elements to pop?")
			else
				ClientConnections.set(endClient, i);
		}
	}
}
