#include "netlib.h"

#ifndef __NETLIB_CLIENT_H__90237423649827469283746928374698273__
#define __NETLIB_CLIENT_H__90237423649827469283746928374698273__

namespace nwol
{
	struct SClientConnection {
		SConnectionEndpoint		* pServer				= 0;			// the remote endpoint
		SConnectionEndpoint		* pClient				= 0;			// us

		int32_t					r1 = 127, r2 = 0, r3 = 0, r4 = 1;	// Remote address components 
		int32_t					rPort = 45678;							// The port number to use
		int32_t					l1 = 0, l2 = 0, l3 = 0, l4 = 0;			// Local address components 
	};

	int32_t					initClientConnection	(SClientConnection& instanceClient);
	int32_t					connect					(SClientConnection& instanceClient);
	int32_t					requestDisconnect		(SClientConnection& client);
	int32_t					disconnectClient		(SClientConnection& client);
	int32_t					serverTime				(SClientConnection& instanceClient, uint64_t& current_time);
} // namespace



#endif // __NETLIB_CLIENT_H__90237423649827469283746928374698273__
