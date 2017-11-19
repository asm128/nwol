#include "netlib_command.h"
#include "nwol_array.h"

#include "gref_definition.h"
#include "glist.h"

GDEFINE_POD(gdnet, SCommandPacket);

bool											gdnet::ping							(SConnectionEndpoints& connection)													{
	// --- Send our command
	int32_t												bytesTransmitted					= -1;
	static const ::gdnet::SCommand						pingCommand							= ::gdnet::SRequest(::gdnet::NWON_COMMAND_PING);
	::nwol::error_t										transmResult						= ::gdnet::connectionSend(connection, (const byte_t*)&pingCommand, sizeof(::gdnet::SCommand), &bytesTransmitted);
	retval_error_if(false, transmResult < 0 || bytesTransmitted != (int32_t)sizeof(::gdnet::SCommand), "Error pinging server.");

	::gdnet::SIPv4										addressRemote						= {};
	::gdnet::endpointAddress(connection.Remote, addressRemote);
	info_printf("Sent ping command to %u.%u.%u.%u:%u.", NWOL_EXPAND_IPV4_UINT32(addressRemote));

	// --- Receive answer
	bytesTransmitted								= -1;
	::gdnet::SCommand									pongCommand							= ::gdnet::INVALID_COMMAND;
	::gdnet::endpointReceive(connection.Local.get_address(), (byte_t*)&pongCommand, sizeof(::gdnet::SCommand), &bytesTransmitted, ::gdnet::RECEIVE_FLAG_REMOVE, 0);
	rve_if(false, pongCommand.Command != ::gdnet::NWON_COMMAND_PING, "Error receiving pong from server.");

	info_printf("Command received from %u.%u.%u.%u:%u: %s:%s.", NWOL_EXPAND_IPV4_UINT32(addressRemote), ::nwol::get_value_label(pongCommand.Type).c_str(), ::nwol::get_value_label(pongCommand.Command).c_str());		
	return true;
}

::nwol::error_t									gdnet::time							(SConnectionEndpoints& connection, uint64_t& current_time)							{
	nwol_necall(::gdnet::commandSend(connection, ::gdnet::SRequest(::gdnet::NWON_COMMAND_TIME_GET)), "%s", "Error transmitting data.");
	::gdnet::SResponseTime								responseTime						= {::gdnet::INVALID_COMMAND};	
	int32_t												bytesTransmitted					= ~0U;
	nwol_necall(::gdnet::endpointReceive(connection.Local.get_address(), (byte_t*)&responseTime, (uint32_t)sizeof(responseTime), &bytesTransmitted, ::gdnet::RECEIVE_FLAG_REMOVE, 0), "%s", "Error receiving data.");
	info_printf("response: %s.", ::nwol::get_value_label(responseTime.Command.Command).c_str());
	current_time									= responseTime.Time;
	return 0;
}

::nwol::error_t									gdnet::commandSend					(SConnectionEndpoints& connection, const ::gdnet::SCommand& commandToSend)	{
	int32_t												sentBytes							= 0;
	::nwol::error_t										errMy								= ::gdnet::connectionSend(connection, (const byte_t*)&commandToSend, (uint32_t)sizeof(::gdnet::SCommand), &sentBytes);
	ree_if(errored(errMy) || sentBytes != (int32_t)sizeof(::gdnet::SCommand), "Error sending system command to remote client. Command: %s.", ::nwol::get_value_label(commandToSend.Command).c_str());
	// --- Log stuff.
	::gdnet::SIPv4										addressRemote						= {};
	::gdnet::endpointAddress(connection.Remote, addressRemote);
	info_printf("Sent sytem command to %u.%u.%u.%u:%u: %s:%s."
		, NWOL_EXPAND_IPV4_UINT32(addressRemote)
		, ::nwol::get_value_label(commandToSend.Type	).c_str()
		, ::nwol::get_value_label(commandToSend.Command	).c_str()
		);
	return 0;
}

::nwol::error_t									gdnet::commandReceive				(SConnectionEndpoints& connection,  ::gdnet::SCommand& commandReceived)		{
	ree_if(0 == connection.Local, "Client listener was terminated.");

	commandReceived									= ::gdnet::INVALID_COMMAND;
	int32_t												bytes_received						= 0;
	nwol_necall(::gdnet::endpointReceive(connection.Local.get_address(), (byte_t*)&commandReceived, sizeof(::gdnet::SCommand), &bytes_received, ::gdnet::RECEIVE_FLAG_REMOVE, 0), "Error receiving system command.");
	ree_if(bytes_received < 0, "Error receiving system command.");
	ree_if(0 == connection.Remote, "Client target was null.");
	::gdnet::SIPv4										addressRemote						= {};
	::gdnet::endpointAddress(connection.Remote, addressRemote);
	info_printf("Received sytem command from %u.%u.%u.%u:%u: %s:%s."
		, NWOL_EXPAND_IPV4_UINT32(addressRemote)
		, ::nwol::get_value_label(commandReceived.Type		).c_str()
		, ::nwol::get_value_label(commandReceived.Command	).c_str()
		);
	return 0;
}

::nwol::error_t									gdnet::commandPeek					(SConnectionEndpoints& connection,  ::gdnet::SCommand& commandReceived)		{
	ree_if(0 == connection.Local, "Client listener was terminated.");

	commandReceived.Command							= ::gdnet::NWON_COMMAND_INVALID;
	int32_t												bytes_received						= 0;
	nwol_necall(::gdnet::endpointReceive(connection.Local.get_address(), (byte_t*)&commandReceived, sizeof(::gdnet::SCommand), &bytes_received, ::gdnet::RECEIVE_FLAG_PEEK, 0), "Error receiving system command.");
	ree_if(bytes_received < 0, "Error receiving system command.");
	ree_if(0 == connection.Remote, "Client target was null.");
	::gdnet::SIPv4										addressRemote						= {};
	::gdnet::endpointAddress(connection.Remote, addressRemote);
	info_printf("Peeked sytem command from %u.%u.%u.%u:%u: %s:%s."
		, NWOL_EXPAND_IPV4_UINT32(addressRemote)
		, ::nwol::get_value_label(commandReceived.Type		).c_str()
		, ::nwol::get_value_label(commandReceived.Command	).c_str()
		);
	return 0;
}

::nwol::error_t									gdnet::dataSend						(::gdnet::SConnectionEndpoints& connection, ::gdnet::NWON_COMMAND_TYPE requestOrResponse, const byte_t* buffer, uint32_t bufferSize) {
	// --- Send data back
	::nwol::array_pod<byte_t>							bufferToSend						= {};
	nwol_necall(bufferToSend.resize(sizeof(::gdnet::SHeaderUserData) + bufferSize), "Failed to resize! Out of memory?");
	::gdnet::SHeaderUserData							* packetToSend						= (::gdnet::SHeaderUserData*)bufferToSend.begin();
	packetToSend->Command							= {::gdnet::NWON_COMMAND_USER_DATA, 0, requestOrResponse};
	packetToSend->PayloadSize						= ::nwol::min((uint16_t)bufferSize, ::gdnet::MAX_COMMAND_PAYLOAD_SIZE);
	::memcpy(&bufferToSend.begin()[sizeof(::gdnet::SHeaderUserData)], buffer, bufferSize);
	int32_t												sentBytes							= 0;
	nwol_necall(::gdnet::connectionSend(connection, (const byte_t*)packetToSend, bufferToSend.size(), &sentBytes), "Error sending user data.");
	::gdnet::SIPv4										addressRemote						= {};
	::gdnet::endpointAddress(connection.Remote, addressRemote);
	info_printf("Sent %u bytes of user data to %u.%u.%u.%u:%u.", bufferSize, NWOL_EXPAND_IPV4_UINT32(addressRemote));
	return 0;
}

::nwol::error_t									gdnet::dataReceive					(::gdnet::SConnectionEndpoints& connection, ::gdnet::NWON_COMMAND_TYPE& requestOrResponse, byte_t* buffer, uint32_t bufferSize) {
	// --- Send data back
	::gdnet::SCommand									commandUserReceive					= ::gdnet::INVALID_COMMAND;
	while(true) {
		nwol_necall(::gdnet::commandPeek(connection, commandUserReceive), "Error receiving system command.");
		if(commandUserReceive.Command != ::gdnet::NWON_COMMAND_USER_DATA)
			continue;
	}
	nwol_necall(::gdnet::commandReceive(connection, commandUserReceive), "Error receiving system command.");
	requestOrResponse								= commandUserReceive.Type;
	ree_if( (commandUserReceive.Type != ::gdnet::NWON_COMMAND_TYPE_REQUEST		)
		&&	(commandUserReceive.Type != ::gdnet::NWON_COMMAND_TYPE_RESPONSE		)
		&&	(commandUserReceive.Type != ::gdnet::NWON_COMMAND_TYPE_ACKNOWLEDGE	)
		, "Command is not valid."
		);
	int32_t												receivedBytes						= 0;
	int32_t												userBytesToReceive					= 0;
	nwol_necall(::gdnet::endpointReceive(connection.Local.get_address(), (byte_t*)&userBytesToReceive, (uint32_t)sizeof(int32_t), &receivedBytes, ::gdnet::RECEIVE_FLAG_REMOVE, 0), "Error receiving user data.");
	ree_if(receivedBytes != (int32_t)sizeof(int32_t), "Error receiving user data.");

	receivedBytes									= 0;
	::nwol::array_pod<byte_t>							userBuffer;
	nwol_necall(userBuffer.resize(userBytesToReceive), "Failed to allocate buffer for network message. Out of memory?");
	nwol_necall(::gdnet::endpointReceive(connection.Local.get_address(), userBuffer.begin(), userBytesToReceive, &receivedBytes, ::gdnet::RECEIVE_FLAG_REMOVE, 0), "Error receiving user data.");
	ree_if(receivedBytes != userBytesToReceive, "Error receiving user data.");

	for(uint32_t iByte = 0, packetByteCount = ::nwol::max(bufferSize, userBuffer.size()); iByte < packetByteCount; ++iByte) 
		buffer[iByte]									= userBuffer[iByte];

	// --- Display time
	::gdnet::SIPv4										addressRemote						= {};
	::gdnet::endpointAddress(connection.Remote, addressRemote);
	info_printf("Received %u bytes of user data from %u.%u.%u.%u:%u.", bufferSize, NWOL_EXPAND_IPV4_UINT32(addressRemote));
	return 0;
}