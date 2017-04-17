#include "netlib_client.h"
#include "netlib_private.h"

#include "nwol_misc.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

int32_t										nwol::initClientConnection					(::nwol::SClientConnection& conn)									{
	::nwol::error_t	
	errMy											= ::nwol::createConnection	(conn.l1, conn.l2, conn.l3, conn.l4, 0, &conn.pClient);						reterr_error_if_errored(errMy, "%s", "Error creating client connection.");
	errMy											= ::nwol::initConnection	(conn.pClient);																reterr_error_if_errored(errMy, "%s", "Error initializing client connection.");
	errMy											= ::nwol::bindConnection	(conn.pClient);																reterr_error_if_errored(errMy, "Error binding client connection. Shutdown: 0x%x.", shutdownConnection(&conn.pClient));
	errMy											= ::nwol::createConnection	(conn.r1, conn.r2, conn.r3, conn.r4, (uint16_t)conn.rPort, &conn.pServer);	reterr_error_if_errored(errMy, "%s", "Error creating connection to new port.");
	return 0;
}

int32_t										nwol::requestDisconnect						(::nwol::SClientConnection& conn)									{
	static const ::nwol::NETLIB_COMMAND					commandDisconnect						= ::nwol::NETLIB_COMMAND_DISCONNECT;
	int32_t												bytesTransmitted						= -1;
	::nwol::error_t										errMy									= ::nwol::sendToConnection( conn.pClient, (const ubyte_t*)&commandDisconnect, (uint32_t)sizeof(::nwol::NETLIB_COMMAND), &bytesTransmitted, conn.pServer );
	reterr_error_if_errored(errMy, "%s", "Error sending disconnect message.");
	reterr_error_if(bytesTransmitted == -1, "%s", "Error sending disconnect message.");
	info_printf("%s", "Disconnect message sent successfully.");
	return 0;
}

int32_t										nwol::disconnectClient						(::nwol::SClientConnection& client)									{
	/* Get current time */
	info_printf("%s", "Disconnecting client.");
	::nwol::shutdownConnection(&client.pClient);
	::nwol::shutdownConnection(&client.pServer);
	return 0;
}

#define MAX_SEND_SIZE 128

int											nwol::connect								(::nwol::SClientConnection& conn)									{
	info_printf("%s", "Sending connection request.");
	static	const ::nwol::NETLIB_COMMAND				send_buffer								= ::nwol::NETLIB_COMMAND_CONNECT;

	int32_t												bytesTransmitted						=-1;
	::nwol::error_t										errMy									= ::nwol::sendToConnection(conn.pClient, (const ubyte_t*)&send_buffer, (uint32_t)sizeof(::nwol::NETLIB_COMMAND), &bytesTransmitted, conn.pServer);
	reterr_error_if_errored(errMy												, "Error transmitting data. Disconnect: 0x%X.", ::nwol::disconnectClient(conn));
	reterr_error_if(bytesTransmitted != (int32_t)sizeof(::nwol::NETLIB_COMMAND)	, "Error transmitting data. Disconnect: 0x%X.", ::nwol::disconnectClient(conn));

	// Get response expecting a remote port to connect to.
	bytesTransmitted								= -1;
	::nwol::NETLIB_COMMAND								connect_buffer							= ::nwol::NETLIB_COMMAND_INVALID;
	errMy											= ::nwol::receiveFromConnection(conn.pClient, (ubyte_t*)&connect_buffer, (uint32_t)sizeof(::nwol::NETLIB_COMMAND), &bytesTransmitted, 0);
	reterr_error_if(connect_buffer != ::nwol::NETLIB_COMMAND_PORT, "Error receiving data. 0x%x, 0x%x", errMy, ::nwol::disconnectClient(conn));
	info_printf("response: %s.", ::nwol::get_value_label(connect_buffer).c_str());

	::nwol::shutdownConnection(&conn.pServer);

	int32_t												port_buffer								= -1;
	errMy											= ::nwol::receiveFromConnection(conn.pClient, (ubyte_t*)&port_buffer, (uint32_t)sizeof(int32_t), &bytesTransmitted, 0);
	reterr_error_if(::nwol::failed(errMy) || (port_buffer == -1), "Error receiving port number. 0x%x, 0x%x", errMy, ::nwol::disconnectClient(conn));
	info_printf("port reported available: %i.", port_buffer);	
	errMy											= ::nwol::createConnection(conn.r1, conn.r2, conn.r3, conn.r4, (uint16_t)port_buffer, &conn.pServer);

	reterr_error_if_errored(errMy, "%s", "Error creating new server connection.");
	info_printf("%s", "Connection created.");
	return 0;
}

int											nwol::serverTime							(::nwol::SClientConnection& conn, uint64_t& current_time)			{
	// send our command
	::nwol::error_t										errMy									= ::nwol::sendSystemCommand(conn.pClient, conn.pServer, ::nwol::NETLIB_COMMAND_TIME_GET);
	reterr_error_if_errored(errMy, "%s", "Error transmitting data.");

	// Receive answer
	::nwol::NETLIB_COMMAND								commandTimeSet							= ::nwol::NETLIB_COMMAND_INVALID;
	errMy											= ::nwol::receiveSystemCommand(conn.pClient, conn.pServer, commandTimeSet);
	reterr_error_if_errored(errMy, "%s", "Error receiving data.");
	
	info_printf("response: %s.", ::nwol::get_value_label(commandTimeSet).c_str());

	uint64_t											receive_time							= ~0ULL;
	int32_t												bytesTransmitted						= ~0U;
	errMy											= ::nwol::receiveFromConnection(conn.pClient, (ubyte_t*)&receive_time, (uint32_t)sizeof(receive_time), &bytesTransmitted, 0);
	reterr_error_if_errored(errMy, "%s", "Error receiving data.");

	memcpy(&current_time, &receive_time, sizeof(uint64_t));
	return 0;
}
