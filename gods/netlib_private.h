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
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, NOP				, 0x000);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, PING				, 0x001);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, PONG				, 0x002);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, TIME_GET			, 0x004);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, TIME_SET			, 0x008);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, CONNECT			, 0x010);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, PORT				, 0x020);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, DISCONNECT		, 0x040);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, USER_REQUEST		, 0x080);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, USER_RESPONSE	, 0x100);
#pragma pack(pop)	

	struct SConnectionEndpoint	{
		SOCKET					sd						= 0;	// Socket descriptor
		sockaddr_in				sockaddr				= {};	// Information about this endpoint
	};

	int32_t					sendSystemCommand		(SConnectionEndpoint* pOrigin	, SConnectionEndpoint* pTarget, const	NETLIB_COMMAND& commandToSend	);
	int32_t					receiveSystemCommand	(SConnectionEndpoint* pLocal	, SConnectionEndpoint* pRemote,			NETLIB_COMMAND& commandReceived	);

} // namespace

#endif // __NETLIB_PRIVATE_H__92364982734698273469823462384238__