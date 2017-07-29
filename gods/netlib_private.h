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
	struct SNetworkEndpoint	{
		SOCKET					sd						= 0;	// Socket descriptor
		sockaddr_in				sockaddr				= {};	// Information about this endpoint
	};

} // namespace

#endif // __NETLIB_PRIVATE_H__92364982734698273469823462384238__