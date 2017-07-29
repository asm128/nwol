#include "netlib.h"

#ifndef __NETLIB_CLIENT_H__90237423649827469283746928374698273__
#define __NETLIB_CLIENT_H__90237423649827469283746928374698273__

namespace nwol
{
	struct SClientConnection {
		SNetworkEndpoint		* pServer				= 0;			// the remote endpoint
		SNetworkEndpoint		* pClient				= 0;			// us

		uint8_t					r1 = 192, r2 = 168, r3 = 1, r4 = 27;	// Remote address components 
		uint16_t				rPort = 45678;							// The port number to use
		uint8_t					l1 = 0, l2 = 0, l3 = 0, l4 = 0;			// Local address components 
	};

	int32_t					initClientConnection	(SClientConnection& instanceClient);
	int32_t					connect					(SClientConnection& instanceClient);
	int32_t					requestDisconnect		(SClientConnection& client);
	int32_t					disconnectClient		(SClientConnection& client);
} // namespace



#endif // __NETLIB_CLIENT_H__90237423649827469283746928374698273__
