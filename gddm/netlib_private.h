#include "netlib.h"
#include "nwol_safe.h"
#include <mutex>

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

#ifndef NWON_PRIVATE_H__92364982734698273469823462384238
#define NWON_PRIVATE_H__92364982734698273469823462384238

namespace gdnet
{
	struct SEndpoint : public IEndpoint	{
		SOCKET					Socket					= INVALID_SOCKET;	// Socket descriptor
		sockaddr_in				SockAddr				= {};				// Information about this endpoint
		TRANSPORT_PROTOCOL		Protocol				= TRANSPORT_PROTOCOL_UDP;

								~SEndpoint				()					{ safe_closesocket(Socket); }
	};
	GDECLARE_OBJ(SEndpoint);
} // namespace

#endif // NWON_PRIVATE_H__92364982734698273469823462384238