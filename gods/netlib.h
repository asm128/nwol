#include "typeint.h"

#ifndef __NETLIB_H__934892734698236498236498716091782__
#define __NETLIB_H__934892734698236498236498716091782__

namespace nwol
{
	struct								SIPV6					{};
	struct								SIPV4					{
											uint8_t					IP	[4]						;	// Address components 
											uint16_t				Port						;							// 
	};

	struct								SNetworkEndpoint;

										int32_t					networkInit					();
										int32_t					networkShutdown				();

										int32_t					endpointCreate				(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3, uint16_t port_number, SNetworkEndpoint** out_createdEndpoint);
										int32_t					endpointCreate				(uint16_t local_port_number, SNetworkEndpoint** out_createdEndpoint);
										int32_t					endpointCreateByHostName	(char_t* host_name, uint16_t port_number, SNetworkEndpoint** out_createdEndpoint);

										int32_t					endpointInit				(SNetworkEndpoint*	endpoint);
										int32_t					endpointShutdown			(SNetworkEndpoint**	endpoint);
										int32_t					endpointBind				(SNetworkEndpoint*	endpoint);

										int32_t					endpointListen				(SNetworkEndpoint*	endpoint);
										int32_t					endpointAccept				(SNetworkEndpoint*	endpoint, SNetworkEndpoint** out_createdEndpoint);

										int32_t					getAddress					(SNetworkEndpoint*	endpointLocal, uint8_t* ip0, uint8_t* ip1, uint8_t* ip2, uint8_t* ip3, uint16_t* port_number);
										int32_t					endpointSend				(SNetworkEndpoint*	endpointLocal, const ubyte_t* buffer, uint32_t bytesToSend	, int32_t* sentBytes	, SNetworkEndpoint*	remote);
										int32_t					endpointReceive				(SNetworkEndpoint*	endpointLocal, ubyte_t*		buffer, uint32_t bufLen			, int32_t* receivedBytes, SNetworkEndpoint** out_createdRemoteEndpoint);

	static inline						int32_t					getAddress					(SNetworkEndpoint*	endpointLocal, SIPV4 &address)																										{ return getAddress(endpointLocal, &address.IP[0], &address.IP[1], &address.IP[2], &address.IP[3], &address.Port);		}
	static inline						int32_t					endpointCreate				(SIPV4 &address, SNetworkEndpoint**	out_createdEndpoint)																								{ return endpointCreate(address.IP[0], address.IP[1], address.IP[2], address.IP[3], address.Port, out_createdEndpoint);	}
}// namespace

#endif // __NETLIB_H__934892734698236498236498716091782__
