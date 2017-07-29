#include "netlib.h"
#include "nwol_enum.h"

#ifndef NETLIB_COMMAND_H_9278340928374
#define NETLIB_COMMAND_H_9278340928374

namespace nwol {
	
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

										int32_t					commandSend					(SNetworkEndpoint* endpointLocal, SNetworkEndpoint* endpointRemote, const	NETLIB_COMMAND& commandToSend	);
										int32_t					commandReceive				(SNetworkEndpoint* endpointLocal, SNetworkEndpoint* endpointRemote,			NETLIB_COMMAND& commandReceived	);
																		
										bool_t					ping						(SNetworkEndpoint* endpointLocal, SNetworkEndpoint* endpointRemote);
										int32_t					time						(SNetworkEndpoint* endpointLocal, SNetworkEndpoint* endpointRemote, uint64_t& current_time);

	GDEFINE_ENUM_TYPE(USER_COMMAND, uint8_t);
	GDEFINE_ENUM_VALUE(USER_COMMAND, UNKNOWN	, 0);
	GDEFINE_ENUM_VALUE(USER_COMMAND, REQUEST	, 1);
	GDEFINE_ENUM_VALUE(USER_COMMAND, RESPONSE	, 2);

										int32_t					dataSend					(SNetworkEndpoint* endpointLocal, SNetworkEndpoint* endpointRemote, const USER_COMMAND	requestOrResponse	, const ubyte_t	* sourceBuffer, uint32_t bufferSize);
										int32_t					dataReceive					(SNetworkEndpoint* endpointLocal, SNetworkEndpoint* endpointRemote, USER_COMMAND			& requestOrResponse	, ubyte_t		* targetBuffer, uint32_t bufferSize);
	template<typename _tPOD>	inline	int32_t					dataSend					(SNetworkEndpoint* endpointLocal, SNetworkEndpoint* endpointRemote, const USER_COMMAND	requestOrResponse	, const _tPOD	& podSourceInstance)					{ return dataSend	(pOrigin, pTarget, requestOrResponse, &podSourceInstance, sizeof(_tPOD)); }
	template<typename _tPOD>	inline	int32_t					dataReceive					(SNetworkEndpoint* endpointLocal, SNetworkEndpoint* endpointRemote, USER_COMMAND			& requestOrResponse	, _tPOD			& podTargetInstance)					{ return dataReceive	(pOrigin, pTarget, requestOrResponse, &podTargetInstance, sizeof(_tPOD)); }
}

#endif  // NETLIB_COMMAND_H_9278340928374