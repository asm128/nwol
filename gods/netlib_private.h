#include "netlib.h"
#if defined(__WINDOWS__)
#include <WinSock2.h>
#include <Ws2tcpip.h> 
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
typedef		int				SOCKET;
#define INVALID_SOCKET		(SOCKET)(~0)
#endif

#include "nwol_enum.h"

#ifndef __NETLIB_PRIVATE_H__92364982734698273469823462384238__
#define __NETLIB_PRIVATE_H__92364982734698273469823462384238__

namespace nwol
{
#pragma pack( push, 1 )
	GDEFINE_ENUM_TYPE (NETLIB_COMMAND, int16_t);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, NOOP				, 0x00);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, PING				, 0x01);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, PONG				, 0x02);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, TIME_GET			, 0x03);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, TIME_SET			, 0x04);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, CONNECT			, 0x05);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, PORT				, 0x06);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, DISCONNECT		, 0x07);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, USER_REQUEST		, 0x08);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, USER_RESPONSE	, 0x09);
#pragma pack(pop)	

	struct SConnectionEndpoint	{
		SOCKET					sd						= 0;	// Socket descriptor
		sockaddr_in				sockaddr				= {};	// Information about this endpoint
	};

	int32_t					sendSystemCommand		(SConnectionEndpoint* pOrigin	, SConnectionEndpoint* pTarget, const	NETLIB_COMMAND& commandToSend	);
	int32_t					receiveSystemCommand	(SConnectionEndpoint* pLocal	, SConnectionEndpoint* pRemote,			NETLIB_COMMAND& commandReceived	);

} // namespace

#endif // __NETLIB_PRIVATE_H__92364982734698273469823462384238__