#include "nwol_error.h"
#include "gref.h"
#include "gref_ptr.h"

#ifndef NETLIB_H__934892734698236498236498716091782
#define NETLIB_H__934892734698236498236498716091782

namespace gdnet
{
	enum								TRANSPORT_PROTOCOL		
		{ TRANSPORT_PROTOCOL_UDP			= 0
		, TRANSPORT_PROTOCOL_TCP			
		};

	struct								IEndpoint				{};
	GDECLARE_PURE(IEndpoint);

										::nwol::error_t			endpointSystemInitialize					();
										::nwol::error_t			endpointSystemShutdown						();

										::nwol::error_t			endpointCreate								(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3, uint16_t addressPort, TRANSPORT_PROTOCOL mode, GODS(IEndpoint)* out_endpointCreated);
										::nwol::error_t			endpointCreate								(uint16_t localPort, uint32_t indexAdapter, TRANSPORT_PROTOCOL mode, GODS(IEndpoint)* out_endpointCreated);

										::nwol::error_t			endpointInit								(IEndpoint*	endpoint);
										::nwol::error_t			endpointBind								(IEndpoint*	endpoint);

										::nwol::error_t			endpointListen								(IEndpoint*	endpoint);
										::nwol::error_t			endpointAccept								(IEndpoint*	endpoint, GODS(IEndpoint)* out_endpointCreated);

										::nwol::error_t			endpointAddress								(IEndpoint*	endpointLocal, uint8_t* ip0, uint8_t* ip1, uint8_t* ip2, uint8_t* ip3, uint16_t* addressPort);
										::nwol::error_t			endpointAddress								(const char_t* hostName, uint16_t addressPort, uint32_t indexAdapter, TRANSPORT_PROTOCOL mode, uint8_t* ip0, uint8_t* ip1, uint8_t* ip2, uint8_t* ip3);
										::nwol::error_t			endpointSend								(IEndpoint*	endpointLocal, const byte_t* buffer, uint32_t bytesToSend, int32_t* bytesSent, IEndpoint* endpointRemote);

	enum								RECEIVE_FLAG 
		{	RECEIVE_FLAG_REMOVE				= 0
		,	RECEIVE_FLAG_PEEK	
		};
										::nwol::error_t			endpointReceive								(IEndpoint*	endpointLocal, byte_t* buffer, uint32_t bufLen, int32_t* bytesReceived, RECEIVE_FLAG receiveFlag, GODS(IEndpoint)* out_endpointRemote);

	struct								SIPv6					{
											uint16_t				IP	[8]										;
	};

	struct								SIPv4					{
											uint8_t					IP	[4]										;	// Address components 
											uint16_t				Port										;							// 
	};

#define NWOL_EXPAND_IPV4_RAW(varName)		varName.IP[0], varName.IP[1], varName.IP[2], varName.IP[3], varName.Port
#define NWOL_EXPAND_IPV4_UINT32(varName)	(uint32_t)varName.IP[0], (uint32_t)varName.IP[1], (uint32_t)varName.IP[2], (uint32_t)varName.IP[3], (uint32_t)varName.Port
#define NWOL_EXPAND_IPV4_POINTER(varName)	&varName.IP[0], &varName.IP[1], &varName.IP[2], &varName.IP[3], &varName.Port

	static inline						::nwol::error_t			endpointAddress								(IEndpoint*	endpointLocal, SIPv4 &address)																											{ return endpointAddress(endpointLocal, NWOL_EXPAND_IPV4_POINTER(address)); }
	static inline						::nwol::error_t			endpointAddress								(const char_t* hostName, uint16_t addressPort, uint32_t indexAdapter, TRANSPORT_PROTOCOL mode, SIPv4 &address)										{ address.Port = addressPort; return endpointAddress(hostName, addressPort, indexAdapter, mode, &address.IP[0], &address.IP[1], &address.IP[2], &address.IP[3]); }
	static inline						::nwol::error_t			endpointCreate								(const SIPv4 &address, TRANSPORT_PROTOCOL mode, GODS(IEndpoint)* out_endpointCreated)																{ return endpointCreate(NWOL_EXPAND_IPV4_RAW(address), mode, out_endpointCreated); }
	static inline						::nwol::error_t			endpointCreate								(const char_t* hostName, uint16_t addressPort, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, ::gdnet::GODS(IEndpoint)* out_endpointCreated)		{
		::gdnet::SIPv4									localAddress;
		::gdnet::endpointAddress(hostName, addressPort, adapterIndex, mode, localAddress);
		return ::gdnet::endpointCreate(localAddress, mode, out_endpointCreated);
	}


	static inline						::nwol::error_t			endpointInit								(GPNCO(::gdnet, IEndpoint)& endpoint)																												{ return endpointInit	(endpoint.get_address()); }
	static inline						::nwol::error_t			endpointBind								(GPNCO(::gdnet, IEndpoint)& endpoint)																												{ return endpointBind	(endpoint.get_address()); }
	static inline						::nwol::error_t			endpointAddress								(GPNCO(::gdnet, IEndpoint)& endpointLocal, SIPv4 &address)																							{ return endpointAddress(endpointLocal.get_address(), NWOL_EXPAND_IPV4_POINTER(address)); }
	static inline						::nwol::error_t			endpointAddress								(GPNCO(::gdnet, IEndpoint)& endpointLocal, uint8_t* ip0, uint8_t* ip1, uint8_t* ip2, uint8_t* ip3, uint16_t* addressPort)							{ return endpointAddress(endpointLocal.get_address(), ip0, ip1, ip2, ip3, addressPort); }
	static inline						::nwol::error_t			endpointSend								(GPNCO(::gdnet, IEndpoint)& endpointLocal, const byte_t* buffer, uint32_t bytesToSend, int32_t* bytesSent, GPNCO(::gdnet, IEndpoint)& endpointRemote)	{ return endpointSend	(endpointLocal.get_address(), buffer, bytesToSend, bytesSent, endpointRemote.get_address()); }
}// namespace

#endif // NETLIB_H__934892734698236498236498716091782
