#include "netlib_command.h"

#include "gbuffer.h"

bool											nwol::ping							(::nwol::SNetworkEndpoint* pClient, ::nwol::SNetworkEndpoint* pServer) {
	// send our command
	int32_t												bytesTransmitted					= -1;
	static const ::nwol::NETLIB_COMMAND					pingCommand							= ::nwol::NETLIB_COMMAND_PING;
	::nwol::error_t										transmResult						= ::nwol::endpointSend( pClient, (const ubyte_t*)&pingCommand, sizeof(::nwol::NETLIB_COMMAND), &bytesTransmitted, pServer );
	retval_error_if(false, transmResult < 0 || bytesTransmitted != (int32_t)sizeof(::nwol::NETLIB_COMMAND), "Error pinging server.");

	uint16_t											port_number							= 0;
	uint8_t												a1									= 0
		,												a2									= 0
		,												a3									= 0
		,												a4									= 0
		;
	::nwol::getAddress( pServer, &a1, &a2, &a3, &a4, &port_number );
	info_printf("Sent ping command to %u.%u.%u.%u:%u.", (int)a1, (int)a2, (int)a3, (int)a4, (int)port_number);

	// Receive answer
	bytesTransmitted								= -1;
	::nwol::NETLIB_COMMAND								pongCommand							= ::nwol::NETLIB_COMMAND_INVALID;
	::nwol::endpointReceive( pClient, (ubyte_t*)&pongCommand, sizeof(::nwol::NETLIB_COMMAND), &bytesTransmitted, 0 );
	retval_error_if(false, pongCommand != ::nwol::NETLIB_COMMAND_PONG, "Error receiving pong from server.");

	info_printf("Received pong command from %u.%u.%u.%u:%u.", (uint32_t)a1, (uint32_t)a2, (uint32_t)a3, (uint32_t)a4, (uint32_t)port_number);
	info_printf("Command received: %s", ::nwol::get_value_label(pongCommand).c_str());		
	return true;
}

int												nwol::time							(::nwol::SNetworkEndpoint* pClient, ::nwol::SNetworkEndpoint* pServer, uint64_t& current_time) {
	nwol_necall(::nwol::commandSend(pClient, pServer, ::nwol::NETLIB_COMMAND_TIME_GET), "%s", "Error transmitting data.");

	::nwol::NETLIB_COMMAND								commandTimeSet						= ::nwol::NETLIB_COMMAND_INVALID;	
	nwol_necall(::nwol::commandReceive(pClient, pServer, commandTimeSet), "%s", "Error receiving data.");
	info_printf("response: %s.", ::nwol::get_value_label(commandTimeSet).c_str());

	uint64_t											receive_time						= ~0ULL;
	int32_t												bytesTransmitted					= ~0U;
	nwol_necall(::nwol::endpointReceive(pClient, (ubyte_t*)&receive_time, (uint32_t)sizeof(receive_time), &bytesTransmitted, 0), "%s", "Error receiving data.");

	memcpy(&current_time, &receive_time, sizeof(uint64_t));
	return 0;
}

int32_t											nwol::commandSend				(::nwol::SNetworkEndpoint* pOrigin, ::nwol::SNetworkEndpoint* pTarget, const ::nwol::NETLIB_COMMAND& commandToSend) {
	int32_t												sentBytes							= 0;
	::nwol::error_t										errMy								= endpointSend(pOrigin, (const ubyte_t*)&commandToSend, (uint32_t)sizeof(::nwol::NETLIB_COMMAND), &sentBytes, pTarget);
	if(errored(errMy) || sentBytes != (int32_t)sizeof(::nwol::NETLIB_COMMAND)) {
		error_printf("Error sending system command to remote client. Command: %s.", ::nwol::get_value_label( commandToSend ).c_str());
		return -1;
	}
	//
	uint16_t											port_number							= 0;
	uint8_t												a1									= 0
		,												a2									= 0
		,												a3									= 0
		,												a4									= 0
		;
	::nwol::getAddress( pTarget, &a1, &a2, &a3, &a4, &port_number );
	info_printf("Sent sytem command to %u.%u.%u.%u:%u: %s.", (uint32_t)a1, (uint32_t)a2, (uint32_t)a3, (uint32_t)a4, (uint32_t)port_number, ::nwol::get_value_label( commandToSend ).c_str());
	return 0;
}

int32_t											nwol::commandReceive			(::nwol::SNetworkEndpoint* pLocal, ::nwol::SNetworkEndpoint* pRemote,  ::nwol::NETLIB_COMMAND& commandReceived) {
	reterr_error_if(0 == pLocal, "Client listener was terminated.");

	commandReceived									= ::nwol::NETLIB_COMMAND_INVALID;
	int32_t												bytes_received						= 0;
	nwol_necall(::nwol::endpointReceive( pLocal, (ubyte_t*)&commandReceived, sizeof(::nwol::NETLIB_COMMAND), &bytes_received, 0 )	, "Error receiving system command.");
	reterr_error_if(bytes_received < 0																									, "Error receiving system command.");

	reterr_error_if(0 == pRemote, "Client target was null.");
	uint16_t											port_number							= 0;
	uint8_t												a1									= 0
		,												a2									= 0
		,												a3									= 0
		,												a4									= 0
		;
	::nwol::getAddress( pRemote, &a1, &a2, &a3, &a4, &port_number );
	info_printf("Received sytem command from %u.%u.%u.%u:%u: %s.", (uint32_t)a1, (uint32_t)a2, (uint32_t)a3, (uint32_t)a4, (uint32_t)port_number, ::nwol::get_value_label( commandReceived ).c_str());
	return 0;
}

int32_t											nwol::dataSend					(SNetworkEndpoint* pOrigin, ::nwol::SNetworkEndpoint* pTarget, ::nwol::USER_COMMAND requestOrResponse, const ubyte_t* buffer, uint32_t bufferSize) {
	// Send data back
	const ::nwol::NETLIB_COMMAND						commandUserSend						= (requestOrResponse == ::nwol::USER_COMMAND_REQUEST) ? ::nwol::NETLIB_COMMAND_USER_REQUEST : ::nwol::NETLIB_COMMAND_USER_RESPONSE;
	nwol_necall(::nwol::commandSend(pOrigin, pTarget, commandUserSend), "Error sending system command.");

	int32_t							
	sentBytes										= 0;
	nwol_necall(::nwol::endpointSend(pOrigin, (const ubyte_t*)&bufferSize, (uint32_t)sizeof(int32_t), &sentBytes, pTarget )	, "Error sending user data.");
	reterr_error_if(sentBytes != (int32_t)sizeof(int32_t)																	, "Error sending user data.");

	sentBytes										= 0;
	nwol_necall(::nwol::endpointSend(pOrigin, buffer, bufferSize, &sentBytes, pTarget)	, "Error sending user data.");
	reterr_error_if(sentBytes != (int32_t)bufferSize									, "Error sending user data.");

	// Display time
	uint16_t											port_number							= 0;
	uint8_t												a1									= 0
		,												a2									= 0
		,												a3									= 0
		,												a4									= 0
		;
	::nwol::getAddress( pTarget, &a1, &a2, &a3, &a4, &port_number );
	info_printf("Sent user command of %u bytes to %u.%u.%u.%u:%u.", bufferSize, (uint32_t)a1, (uint32_t)a2, (uint32_t)a3, (uint32_t)a4, (uint32_t)port_number);
	return 0;
}

int32_t										nwol::dataReceive			(::nwol::SNetworkEndpoint* pOrigin, ::nwol::SNetworkEndpoint* pTarget, ::nwol::USER_COMMAND& requestOrResponse, ubyte_t* buffer, uint32_t bufferSize) {
	// Send data back
	::nwol::NETLIB_COMMAND							commandUserReceive					= ::nwol::NETLIB_COMMAND_INVALID;
	nwol_necall(::nwol::commandReceive(pOrigin, pTarget, commandUserReceive), "Error receiving system command.");

	requestOrResponse	= (::nwol::USER_COMMAND)
		( (commandUserReceive == ::nwol::NETLIB_COMMAND_USER_REQUEST											) ? ::nwol::USER_COMMAND_REQUEST 
		: (commandUserReceive == ::nwol::NETLIB_COMMAND_USER_RESPONSE											) ? ::nwol::USER_COMMAND_RESPONSE 
		: (commandUserReceive == (::nwol::NETLIB_COMMAND_USER_RESPONSE | ::nwol::NETLIB_COMMAND_USER_RESPONSE)	) ? (::nwol::USER_COMMAND_RESPONSE | ::nwol::NETLIB_COMMAND_USER_REQUEST)
		: ::nwol::USER_COMMAND_UNKNOWN
		);
	reterr_error_if(requestOrResponse == ::nwol::USER_COMMAND_UNKNOWN, "Command is not valid.");
	int32_t											receivedBytes						= 0;
	int32_t											userBytesToReceive					= 0;
	nwol_necall(::nwol::endpointReceive( pOrigin, (ubyte_t*)&userBytesToReceive, (uint32_t)sizeof(int32_t), &receivedBytes, &pTarget )	, "Error receiving user data.");
	reterr_error_if(receivedBytes != (int32_t)sizeof(int32_t)																				, "Error receiving user data.");

	receivedBytes								= 0;
	::nwol::gauchar									userBuffer							(userBytesToReceive);
	reterr_error_if(userBuffer.size() != (uint32_t)userBytesToReceive, "Failed to allocate buffer for network message.");
	nwol_necall(::nwol::endpointReceive( pOrigin, userBuffer.begin(), userBytesToReceive, &receivedBytes, &pTarget )	, "Error receiving user data.");
	reterr_error_if(receivedBytes != userBytesToReceive																		, "Error receiving user data.");

	for(uint32_t iByte=0, packetByteCount = ::nwol::max(bufferSize, userBuffer.size()); iByte < packetByteCount; ++iByte) 
		buffer[iByte]								= userBuffer[iByte];

	// Display time
	uint16_t										port_number							= 0;
	uint8_t											a1									= 0
		,											a2									= 0
		,											a3									= 0
		,											a4									= 0
		;	
	::nwol::getAddress( pTarget, &a1, &a2, &a3, &a4, &port_number );
	info_printf("Sent user command of %u bytes to %u.%u.%u.%u:%u.", bufferSize, (uint32_t)a1, (uint32_t)a2, (uint32_t)a3, (uint32_t)a4, (uint32_t)port_number);
	return 0;
}