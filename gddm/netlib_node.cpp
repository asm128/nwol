#include "netlib_node.h"

#include "gref_definition.h"

#include <time.h>

#if defined(__WINDOWS__)
#	include <process.h>
#endif

GDEFINE_OBJ(gdnet, SNetworkNode);

::nwol::error_t										gdnet::nodeInit									(::gdnet::SNetworkNode& node, const ::gdnet::SConnectionAddress& addresses, ::gdnet::TRANSPORT_PROTOCOL mode, uint64_t nodeId)			{
	node.InputRequests									= {};
	node.InputResponses									= {};
	::gdnet::SConnectionEndpoints							endpoints;
	nwol_necall(::gdnet::endpointCreate	(addresses.Local, mode, &endpoints.Local)	, "%s", "Error creating local endpoint."				);
	nwol_necall(::gdnet::endpointInit	(endpoints.Local)							, "%s", "Error initializing local endpoint."			);
	nwol_necall(::gdnet::endpointBind	(endpoints.Local)							, "%s", "Error binding local endpoint.	"				);
	nwol_necall(::gdnet::endpointCreate	(addresses.Remote, mode, &endpoints.Remote)	, "%s", "Error creating remote endpoint to address."	);
	node.Endpoints										= endpoints;
	node.Address										= addresses;
	node.Id												= nodeId;
	info_printf("Client initialized.");
	return 0;
}

static	::nwol::error_t								listenProcedure									(::gdnet::SNetworkNode * pNode);
static	void										listenProcedureThread							(void *pvNodeRef)																					{
	::gdnet::GODS(SNetworkNode)								pClient											= (::gdnet::GODS(SNetworkNode))pvNodeRef;
#ifndef NWOL_DISABLE_EXCEPTIONS
	try {
#endif
	error_if(errored(::listenProcedure(pClient->get())), "Listen procedure exited with error.");
#ifndef NWOL_DISABLE_EXCEPTIONS
	}
	catch(::std::exception e) {
		error_printf("Someone has thrown an ::std::exception(): '%s'.", e.what());
	}
	catch(...) {
		error_printf("Someone has thrown an exception.");
	}
#endif
	::nwol::release(&pClient);
}

::nwol::error_t										gdnet::nodeInit									(::gdnet::GODS(SNetworkNode) pnoderef, const ::gdnet::SConnectionEndpoints& connection, uint64_t id)	{
	::gdnet::SNetworkNode									* node											= pnoderef->get();
	node->Endpoints										= connection;
	nwol_necall(::gdnet::connectionAddress(node->Endpoints, node->Address), "This should never happen.");
	node->Id											= id;
#if defined(__WINDOWS__)
	::_beginthread(::listenProcedureThread, 0, ::nwol::acquire(pnoderef));
#else
#	error "Not implemented."
#endif
	return 0;
}

::nwol::error_t										gdnet::nodeRequestDisconnect						(::gdnet::SNetworkNode& node)																		{
	static	const ::gdnet::SCommand							commandDisconnect								= ::gdnet::SRequest(::gdnet::NWON_COMMAND_DISCONNECT);
	int32_t													bytesTransmitted								= -1;
	nwol_necall(::gdnet::connectionSend(node.Endpoints, (const byte_t*)&commandDisconnect, (uint32_t)sizeof(::gdnet::SCommand), &bytesTransmitted), "%s", "Error sending disconnect message.");
	ree_if(bytesTransmitted == -1, "%s", "Error sending disconnect message.");
	info_printf("%s", "Disconnect message sent successfully.");
	return 0;
}

::nwol::error_t										gdnet::nodeConnect								(::gdnet::SNetworkNode& node)																		{
	info_printf("%s", "Sending connection request.");
	static	const ::gdnet::SCommand							send_buffer										= ::gdnet::SRequest(::gdnet::NWON_COMMAND_CONNECT);

	int32_t													bytesTransmitted								= -1;
	nwol_necall(::gdnet::connectionSend(node.Endpoints, (const byte_t*)&send_buffer, (uint32_t)sizeof(::gdnet::SCommand), &bytesTransmitted), "Error transmitting data.");
	ree_if(bytesTransmitted != (int32_t)sizeof(::gdnet::SCommand), "Error transmitting data.");

	bytesTransmitted									= -1;
	::gdnet::SResponseConnect								responseConnect									= {::gdnet::INVALID_COMMAND, 0};
	nwol_necall(::gdnet::endpointReceive(node.Endpoints.Local.get_address(), (byte_t*)&responseConnect, (uint32_t)sizeof(::gdnet::SResponseConnect), &bytesTransmitted, ::gdnet::RECEIVE_FLAG_REMOVE, &node.Endpoints.Remote), "Error receiving connect response.");
	info_printf("Received answer: %s:%s."
		, ::nwol::get_value_label(responseConnect.Command.Type		).c_str()
		, ::nwol::get_value_label(responseConnect.Command.Command	).c_str()
		);
	ree_if(responseConnect.Command.Command != ::gdnet::NWON_COMMAND_CONNECT, "Error receiving data.");
	::gdnet::endpointAddress(node.Endpoints.Remote, node.Address.Remote);
	info_printf("%s", "Connection created.");
	return 0;
}

static	::nwol::error_t								processCommandInternal							(::gdnet::SNetworkNode* client, ::gdnet::SCommand command)											{
	info_printf("Processing system command: %s:%s.", ::nwol::get_value_label(command.Type).c_str(), ::nwol::get_value_label(command.Command).c_str());

//	char													receiveSmallBuf		[256]						= {};
		 if (command.Command == ::gdnet::NWON_COMMAND_DISCONNECT	) { debug_print("Disconnect requested by client."); ::gdnet::connectionDisconnect(client->Endpoints);								}
	else if (command.Command == ::gdnet::NWON_COMMAND_PING			) { nwol_necall(::gdnet::commandSend(client->Endpoints, ::gdnet::SResponse(::gdnet::NWON_COMMAND_PING)), "Failed to pong client.");	}	// Pong client 
	else if (command.Command == ::gdnet::NWON_COMMAND_TIME_GET		) {
		::gdnet::SResponseTime									response									= {::gdnet::SResponse(::gdnet::NWON_COMMAND_TIME_GET), (uint64_t)time(0)};
		int32_t													bytesSent									= 0;
		nwol_necall(::gdnet::connectionSend(client->Endpoints, (const char_t*)&response, sizeof(::gdnet::SResponseTime), &bytesSent), "Failed to send time to client.");
		ree_if(bytesSent != (int32_t)sizeof(::gdnet::SResponseTime), "Failed to send time to client.");
#if defined(NWOL_DEBUG_ENABLED)		// Display time 
		char													timestring		[256]							= {};
		time_t													timeWithUnreliableSize							= (time_t)response.Time;
#if defined(__WINDOWS__)
		ctime_s(timestring, ::nwol::size(timestring), &timeWithUnreliableSize);
#else
		strcpy(timestring, ctime(&curTimeWithUnreliableSize));
#endif
		timestring[strlen(timestring)-1]					= 0;
		info_printf("Sent time (%llu) (%s) to %u.%u.%u.%u:%u.", response.Time, timestring, NWOL_EXPAND_IPV4_UINT32(client->Address.Remote));
#endif
	}
	else if (command.Command == ::gdnet::NWON_COMMAND_USER_DATA) {
		::gdnet::SHeaderUserData								commandUserData									= {::gdnet::INVALID_COMMAND};
		int32_t													receivedBytes									= 0;
		nwol_necall(::gdnet::endpointReceive(client->Endpoints.Local.get_address(), (byte_t*)&commandUserData, sizeof(int32_t), &receivedBytes, ::gdnet::RECEIVE_FLAG_PEEK, 0), "Failed to receive payload size from remote connection.");
		uint16_t												payloadSize										= commandUserData.PayloadSize;
		switch(payloadSize) {
		case 1	: { byte_t bufferReceive[sizeof(::gdnet::SHeaderUserData) + 1] = {}; nwol_necall(::gdnet::endpointReceive(client->Endpoints.Local.get_address(), bufferReceive, ::nwol::size(bufferReceive), &receivedBytes, ::gdnet::RECEIVE_FLAG_PEEK, 0), "Failed to receive payload size from remote connection."); return ::nodeHandleUserData(client, (const char*)&bufferReceive[sizeof(::gdnet::SHeaderUserData)], payloadSize); } 
		case 2	: { byte_t bufferReceive[sizeof(::gdnet::SHeaderUserData) + 2] = {}; nwol_necall(::gdnet::endpointReceive(client->Endpoints.Local.get_address(), bufferReceive, ::nwol::size(bufferReceive), &receivedBytes, ::gdnet::RECEIVE_FLAG_PEEK, 0), "Failed to receive payload size from remote connection."); return ::nodeHandleUserData(client, (const char*)&bufferReceive[sizeof(::gdnet::SHeaderUserData)], payloadSize); } 
		case 4	: { byte_t bufferReceive[sizeof(::gdnet::SHeaderUserData) + 4] = {}; nwol_necall(::gdnet::endpointReceive(client->Endpoints.Local.get_address(), bufferReceive, ::nwol::size(bufferReceive), &receivedBytes, ::gdnet::RECEIVE_FLAG_PEEK, 0), "Failed to receive payload size from remote connection."); return ::nodeHandleUserData(client, (const char*)&bufferReceive[sizeof(::gdnet::SHeaderUserData)], payloadSize); } 
		case 8	: { byte_t bufferReceive[sizeof(::gdnet::SHeaderUserData) + 8] = {}; nwol_necall(::gdnet::endpointReceive(client->Endpoints.Local.get_address(), bufferReceive, ::nwol::size(bufferReceive), &receivedBytes, ::gdnet::RECEIVE_FLAG_PEEK, 0), "Failed to receive payload size from remote connection."); return ::nodeHandleUserData(client, (const char*)&bufferReceive[sizeof(::gdnet::SHeaderUserData)], payloadSize); } 
		default	:
		{
			::nwol::array_pod<byte_t>								payloadBytes									= {};
			nwol_necall(payloadBytes.resize(payloadSize + sizeof(::gdnet::SHeaderUserData)), "Out of memory.");
			ree_if(payloadSize >= 0xFFFFU || payloadSize <= 0, "Invalid payload size: %u.", payloadSize);
			nwol_necall(payloadBytes.resize(payloadSize), "Cannot allocate memory for network message. Out of memory?");
			nwol_necall(::gdnet::endpointReceive(client->Endpoints.Local.get_address(), payloadBytes.begin(), payloadBytes.size(), &receivedBytes, ::gdnet::RECEIVE_FLAG_PEEK, 0), "Failed to receive payload size from remote connection.");
			return ::nodeHandleUserData(client, &payloadBytes.begin()[sizeof(::gdnet::SHeaderUserData)], payloadSize);
		}
		}
	}
	else
		nwol_necall(::gdnet::commandSend(client->Endpoints, ::gdnet::SResponse(::gdnet::NWON_COMMAND_INVALID)), "Failed to report invalid command to client.");
	
	return 0;
}

static	::nwol::error_t								listenProcedure									(::gdnet::SNetworkNode * pNode)																		{
	::gdnet::SResponseConnect								responseConnect									= {::gdnet::SResponse(::gdnet::NWON_COMMAND_CONNECT), pNode->Address.Local.Port};
	int32_t													sentBytes										= 0;
	nwol_necall(::gdnet::connectionSend(pNode->Endpoints, (const byte_t*)&responseConnect, sizeof(::gdnet::SResponseConnect), &sentBytes), "Failed to send port number to client: %u.", (uint32_t)pNode->Address.Local.Port);
	ree_if(sentBytes != sizeof(::gdnet::SResponseConnect), "%s", "Failed to send port command to client.");

	::gdnet::SIPv4											addressRemote									= {};
	nwol_necall(::gdnet::endpointAddress(pNode->Endpoints.Remote, addressRemote), "This should never happen.");
	info_printf("Sent available port (%u) to %u.%u.%u.%u:%u.", (uint32_t)pNode->Address.Local.Port, NWOL_EXPAND_IPV4_UINT32(addressRemote));

	do {
		::gdnet::SCommand									command											= {::gdnet::NWON_COMMAND_INVALID, 1, ::gdnet::NWON_COMMAND_TYPE_INVALID};
		be_if(errored(::gdnet::commandPeek(pNode->Endpoints, command)), "Failed to peek command!");
		be_if(errored(::processCommandInternal(pNode, command)), "processCommandInternal failed for command: '%s' (0x%x)(%u).", ::nwol::get_value_label(command.Command).c_str(), (uint32_t)command.Command, (uint32_t)command.Command);
		if(pNode->Endpoints.Local) {
			be_if(errored(::gdnet::commandReceive(pNode->Endpoints, command)), "Failed to receive command from client.");
		}
	}
	while (pNode->Endpoints.Local && pNode->Endpoints.Remote);	// Repeat 

	return 0;
}
