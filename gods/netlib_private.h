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
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, 0x00, NOOP			);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, 0x01, PING			);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, 0x02, PONG			);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, 0x03, TIME_GET		);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, 0x04, TIME_SET		);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, 0x05, CONNECT		);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, 0x06, PORT			);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, 0x07, DISCONNECT		);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, 0x08, USER_REQUEST	);
	GDEFINE_ENUM_VALUE(NETLIB_COMMAND, 0x09, USER_RESPONSE	);
#pragma pack(pop)	

	struct SConnectionEndpoint	{
		SOCKET					sd						= 0;	// Socket descriptor
		sockaddr_in				sockaddr				= {};	// Information about this endpoint
	};

	int32_t					sendSystemCommand		(SConnectionEndpoint* pOrigin	, SConnectionEndpoint* pTarget, const	NETLIB_COMMAND& commandToSend	);
	int32_t					receiveSystemCommand	(SConnectionEndpoint* pLocal	, SConnectionEndpoint* pRemote,			NETLIB_COMMAND& commandReceived	);

} // namespace

#endif // __NETLIB_PRIVATE_H__92364982734698273469823462384238__