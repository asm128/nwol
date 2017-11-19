#include "netlib_server.h"

#include "nwol_sleep.h"
#include "gref_definition.h"

#include <time.h>

#if defined(__WINDOWS__)
#include <process.h>
#include <WinSock2.h>
#endif

void												gdnet::disconnectClient							(SServer& serverInstance, int32_t clientId)									{
	::nwol::CMutexGuard										lockClients										(serverInstance.ConnectionsMutex);
	for(uint32_t i = 0; i < serverInstance.ClientConnections.size(); ++i) {
		::gdnet::SNetworkNode									* client										= serverInstance.ClientConnections[i];
		if(client && client->Id == clientId) {
			GPNCO(::gdnet, SNetworkNode)							endClient										= 0;
			error_if(0 > serverInstance.ClientConnections.pop(&endClient), "This is broken! No elements to pop?")
			else
				re_if(errored(serverInstance.ClientConnections.set(endClient, i)), "Failed to set client! Out of memory?");
		}
	}
}

int32_t												gdnet::SServer::InitServer						(uint16_t listeningPort, ::gdnet::TRANSPORT_PROTOCOL mode)					{
	::gethostname(HostName, sizeof(HostName));	// Get host name of this computer
	QueuedConnectionCount								= 0;
	GPNCO(::gdnet, IEndpoint)								endpointListener								= nullptr;			// Information about the client
	nwol_necall(::gdnet::endpointCreate(HostName, (uint16_t)listeningPort, 0, mode, &endpointListener)	, "Failed to initialize listening endpoint on %u:.%s."	, (uint32_t)listeningPort, ::nwol::get_value_label(mode).begin());
	nwol_necall(::gdnet::endpointInit(endpointListener)													, "Failed to initialize listening endpoint on %u:.%s."	, (uint32_t)listeningPort, ::nwol::get_value_label(mode).begin());
	nwol_necall(::gdnet::endpointBind(endpointListener)													, "Failed to bind listening endpoint on %u:.%s."		, (uint32_t)listeningPort, ::nwol::get_value_label(mode).begin());
	EndpointListener									= endpointListener;
	bListening											= true;
	return 0;
}

int32_t												gdnet::SServer::Listen							()																			{
	if(!bListening)
		return 1;
	::gdnet::SCommand										command											= ::gdnet::INVALID_COMMAND;
	GPNCO(::gdnet, IEndpoint)								endpointToAccept								= nullptr;			// Information about the client
	int32_t													bytes_received									= 0;
	{
		::gdnet::SIPv4											addressListener									= {};
		::gdnet::endpointAddress(EndpointListener, addressListener);
		info_printf("Server listening on %u.%u.%u.%u:%u.", NWOL_EXPAND_IPV4_UINT32(addressListener));
		// -- Receive bytes from client
		::nwol::error_t											errMy											= ::gdnet::endpointReceive( EndpointListener.get_address(), (byte_t*)&command, sizeof(::gdnet::SCommand), &bytes_received, ::gdnet::RECEIVE_FLAG_REMOVE, &endpointToAccept );
		ree_if(bytes_received < 0, "Could not receive datagram. 0x%x.", errMy);
		rww_if(bytes_received == 0, "Server listener may have been closed. 0x%x.", errMy);
	}
	{
		::gdnet::SIPv4											addressToAccept									= {};
		error_if(errored(::gdnet::endpointAddress(endpointToAccept, addressToAccept)), "I have no idea why this would ever fail.");
		info_printf("Received %u bytes from %u.%u.%u.%u:%u. Command: %s:%s.", (uint32_t)bytes_received, NWOL_EXPAND_IPV4_UINT32(addressToAccept)
			, ::nwol::get_value_label(command.Type		).c_str()
			, ::nwol::get_value_label(command.Command	).c_str()
			);

		if(bListening && command.Command == ::gdnet::NWON_COMMAND_CONNECT && QueuedConnectionCount < (int64_t)::nwol::size(QueuedConnectionList))
			QueuedConnectionList[sync_increment(QueuedConnectionCount) - 1]	= endpointToAccept;
	}
	return 0;
}

int32_t												gdnet::SServer::Accept							()																			{
	if( 0 == QueuedConnectionCount )
		return 1;

	static int32_t											connectionsAccepted								= 0;
	::gdnet::SConnectionAddress								address											= {};
	GPNCO(::gdnet, IEndpoint)								endpointListener								= 0;
	GPNCO(::gdnet, IEndpoint)								endpointRemote									= QueuedConnectionList[sync_decrement(QueuedConnectionCount)];
	nwol_necall(::gdnet::endpointAddress	(EndpointListener, address.Local)							, "Failed to resolve local address.");
	nwol_necall(::gdnet::endpointAddress	(endpointRemote, address.Remote)							, "Failed to resolve remote address.");
	address.Local.Port									= 0;	// Let the system pick the port number.
	nwol_necall(::gdnet::endpointCreate		(address.Local, TRANSPORT_PROTOCOL_UDP, &endpointListener)	, "%s", "Failed to create client listener.");
	nwol_necall(::gdnet::endpointInit		(endpointListener)											, "%s", "Failed to initialize client listener connection.");
	nwol_necall(::gdnet::endpointBind		(endpointListener)											, "%s", "Failed to bind client listener connection.");

	GPObj(::gdnet, SNetworkNode)							newClient;
	uint32_t												indexFound										= (uint32_t)-1;
	{
		::nwol::CMutexGuard										lockClients										(ConnectionsMutex);
		for(uint32_t iClient = 0, clientCount = ClientConnections.size(); iClient<clientCount; ++iClient) {
			::gdnet::GREF(SNetworkNode)								* refClient										= ClientConnections.begin()[iClient];
			if(0 == refClient) {
				indexFound											= iClient;
				break;
			}
			else if(refClient->get()->Endpoints.Local == 0 || refClient->get()->Endpoints.Remote == 0) {
				newClient											= ::nwol::acquire(refClient);
				break;
			}
		}
		if(0 == newClient) {
			newClient.create();
			if(indexFound != (uint32_t)-1) 
				nwol_necall(ClientConnections.set(newClient, indexFound), "Failed to set client to connection list! Why would this ever happen?");
			else
				nwol_necall(ClientConnections.push_back(newClient), "Failed to push client connection. Out of memory?");
		}
		uint64_t												newClientId										= connectionsAccepted++;
		nwol_necall(::gdnet::nodeInit(newClient, {endpointListener, endpointRemote}, newClientId), "Failed to initialize node. This may be due to the socket being closed before fully initializing.");
	}
	Sleep(10);
	return 0;
};

::nwol::error_t										gdnet::SServer::ShutdownServer					()																			{
	bListening											= false;
	EndpointListener									= {};
	::nwol::sleep(100);

	while(QueuedConnectionCount > 0)
		QueuedConnectionList[sync_decrement(QueuedConnectionCount)] = 0;

	{
		::nwol::CMutexGuard										lockClients										(ConnectionsMutex);
		for(uint32_t iClient = 0, clientCount = ClientConnections.size(); iClient < clientCount; ++iClient) {
			::gdnet::SNetworkNode									* nodeToDisconnect								= ClientConnections[iClient];
			if (nodeToDisconnect)
				nodeToDisconnect->Endpoints							= {};
		}
		::nwol::sleep(100);
	}
	return 0;
}
