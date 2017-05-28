#include "nwol_enum.h"

#ifndef __NETLIB_H__934892734698236498236498716091782__
#define __NETLIB_H__934892734698236498236498716091782__

namespace nwol
{
	struct								SIPV6					{};
	struct								SIPV4					{
											ubyte_t					addr[4]						= {127, 0, 0, 1};	// Address components 
											uint16_t				Port						= 45678;							// 
	};

	struct								SConnectionEndpoint;

										int32_t					initNetwork					();
										int32_t					shutdownNetwork				();

										int32_t					createConnection			(int32_t b1, int32_t b2, int32_t b3, int32_t b4, uint16_t port_number, SConnectionEndpoint** out_newConnection);
										int32_t					createConnection			(uint16_t port_number, SConnectionEndpoint** out_clientInfo);
										int32_t					createConnectionByHostName	(char_t* host_name, uint16_t port_number, SConnectionEndpoint** out_newConnection);

										int32_t					initConnection				(SConnectionEndpoint*	connection);
										int32_t					shutdownConnection			(SConnectionEndpoint**	connection);
										int32_t					bindConnection				(SConnectionEndpoint*	connection);

										int32_t					connectionListen			(SConnectionEndpoint* connection);
										int32_t					connectionAccept			(SConnectionEndpoint* connection, SConnectionEndpoint** newConnection);

										int32_t					getAddress					(SConnectionEndpoint* connection, int32_t* a1, int32_t* a2, int32_t* a3, int32_t* a4, int32_t* port_number);
										int32_t					sendToConnection			(SConnectionEndpoint* connection, const ubyte_t* buffer, uint32_t bytesToSend	, int32_t* sentBytes	, SConnectionEndpoint*	targetConnection);
										int32_t					receiveFromConnection		(SConnectionEndpoint* connection, ubyte_t*		buffer, uint32_t bufLen			, int32_t* receivedBytes, SConnectionEndpoint** newConnection	);
										bool_t					ping						(SConnectionEndpoint* pClient, SConnectionEndpoint* pServer);

	GDEFINE_ENUM_TYPE(USER_COMMAND, uint8_t);
	GDEFINE_ENUM_VALUE(USER_COMMAND, UNKNOWN	, 0);
	GDEFINE_ENUM_VALUE(USER_COMMAND, REQUEST	, 1);
	GDEFINE_ENUM_VALUE(USER_COMMAND, RESPONSE	, 2);

										int32_t					sendUserCommand				(SConnectionEndpoint* pOrigin, SConnectionEndpoint* pTarget, const USER_COMMAND	requestOrResponse	, const ubyte_t	* sourceBuffer, uint32_t bufferSize);
										int32_t					receiveUserCommand			(SConnectionEndpoint* pOrigin, SConnectionEndpoint* pTarget, USER_COMMAND		& requestOrResponse	, ubyte_t		* targetBuffer, uint32_t bufferSize);
	template<typename _tPOD>	inline	int32_t					sendUserCommand				(SConnectionEndpoint* pOrigin, SConnectionEndpoint* pTarget, const USER_COMMAND	requestOrResponse	, const _tPOD	& podSourceInstance)			{ return sendUserCommand	(pOrigin, pTarget, requestOrResponse, &podSourceInstance, sizeof(_tPOD)); }
	template<typename _tPOD>	inline	int32_t					receiveUserCommand			(SConnectionEndpoint* pOrigin, SConnectionEndpoint* pTarget, USER_COMMAND		& requestOrResponse	, _tPOD			& podTargetInstance)			{ return receiveUserCommand	(pOrigin, pTarget, requestOrResponse, &podTargetInstance, sizeof(_tPOD)); }
}// namespace

#endif // __NETLIB_H__934892734698236498236498716091782__
