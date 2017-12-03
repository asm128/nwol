#include "netlib.h"
#include "nwol_enum.h"
#include "gref.h"
#include "gref_ptr.h"

#ifndef NWON_COMMAND_H_9278340928374
#define NWON_COMMAND_H_9278340928374

namespace gdnet {
	
#pragma pack( push, 1 )
	GDEFINE_ENUM_TYPE (NWON_COMMAND, uint8_t);
	GDEFINE_ENUM_VALUE(NWON_COMMAND, NOP				, 0x00);
	GDEFINE_ENUM_VALUE(NWON_COMMAND, PING				, 0x01);
	GDEFINE_ENUM_VALUE(NWON_COMMAND, ECHO				, 0x02);
	GDEFINE_ENUM_VALUE(NWON_COMMAND, PORT_LOCAL			, 0x03);
	GDEFINE_ENUM_VALUE(NWON_COMMAND, PORT_REMOTE		, 0x04);
	GDEFINE_ENUM_VALUE(NWON_COMMAND, TIME_GET			, 0x05);
	GDEFINE_ENUM_VALUE(NWON_COMMAND, CONNECT			, 0x06);
	GDEFINE_ENUM_VALUE(NWON_COMMAND, DISCONNECT			, 0x07);
	GDEFINE_ENUM_VALUE(NWON_COMMAND, USER_DATA			, 0x08);

	GDEFINE_ENUM_TYPE(NWON_COMMAND_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(NWON_COMMAND_TYPE, UNKNOWN		, 0);
	GDEFINE_ENUM_VALUE(NWON_COMMAND_TYPE, REQUEST		, 1);
	GDEFINE_ENUM_VALUE(NWON_COMMAND_TYPE, ACKNOWLEDGE	, 2);
	GDEFINE_ENUM_VALUE(NWON_COMMAND_TYPE, RESPONSE		, 3);

	struct SCommand {
											NWON_COMMAND				Command											: 4;
											uint8_t						Reserved										: 1;
											NWON_COMMAND_TYPE			Type											: 3;
	};
	static constexpr					const SCommand				INVALID_COMMAND									= SCommand{(NWON_COMMAND)0xF, 1, (NWON_COMMAND_TYPE)7};

	struct SHeaderUserData {
											SCommand					Command		;
											uint16_t					PayloadSize	;
	};


	struct SResponseConnect {
											SCommand					Command		;
											uint16_t					Port		;
	};

	struct SResponseTime {
											SCommand					Command		;
											uint64_t					Time		;
	};

	struct SResponsePing {
											SCommand					Command		;
	};

#pragma pack(pop)
	struct SRequest		{ SCommand Command; inline constexpr		SRequest										(::gdnet::NWON_COMMAND command) : Command{command, 0, ::gdnet::NWON_COMMAND_TYPE_REQUEST		} {} inline constexpr operator const SCommand& () { return Command; } };
	struct SResponse	{ SCommand Command; inline constexpr		SResponse										(::gdnet::NWON_COMMAND command) : Command{command, 0, ::gdnet::NWON_COMMAND_TYPE_RESPONSE		} {} inline constexpr operator const SCommand& () { return Command; } };
	struct SAcknowledge	{ SCommand Command; inline constexpr		SAcknowledge									(::gdnet::NWON_COMMAND command) : Command{command, 0, ::gdnet::NWON_COMMAND_TYPE_ACKNOWLEDGE	} {} inline constexpr operator const SCommand& () { return Command; } };


	struct				SConnectionEndpoints	{
											GPNCO(::gdnet, IEndpoint)	Local	;
											GPNCO(::gdnet, IEndpoint)	Remote	;
	};

	struct				SConnectionAddress		{
											::gdnet::SIPv4				Local	;
											::gdnet::SIPv4				Remote	;
	};


	static inline						::nwol::error_t				connectionSend									(::gdnet::SConnectionEndpoints& connection, const byte_t* buffer, uint32_t bytesToSend, int32_t* bytesSent)										{ return endpointSend(connection.Local.get_address(), buffer, bytesToSend, bytesSent, connection.Remote.get_address()); }
	static inline						::nwol::error_t				connectionDisconnect							(::gdnet::SConnectionEndpoints& connection)																										{ connection = {}; return 0; }
	static inline						::nwol::error_t				connectionAddress								(::gdnet::SConnectionEndpoints& connection, SConnectionAddress& address)																			{
		return errored(::gdnet::endpointAddress(connection.Local	, address.Local)) 
			|| errored(::gdnet::endpointAddress(connection.Remote	, address.Remote))
			? -1 : 0;
	}

	//static inline						::nwol::error_t				endpointReceive									(::gdnet::SConnectionEndpoints& connection, byte_t* buffer, uint32_t bufLen, int32_t* bytesReceived, RECEIVE_FLAG receiveFlag);


										int32_t						commandSend										(::gdnet::SConnectionEndpoints& connection, const	SCommand& commandToSend		);
										int32_t						commandReceive									(::gdnet::SConnectionEndpoints& connection,			SCommand& commandReceived	);
										int32_t						commandPeek										(::gdnet::SConnectionEndpoints& connection,			SCommand& commandReceived	);

										bool_t						ping											(::gdnet::SConnectionEndpoints& connection);
										int32_t						time											(::gdnet::SConnectionEndpoints& connection, uint64_t& current_time);

										int32_t						dataSend										(::gdnet::SConnectionEndpoints& connection, const NWON_COMMAND_TYPE	requestOrResponse	, const byte_t	* sourceBuffer, uint32_t bufferSize);
										int32_t						dataReceive										(::gdnet::SConnectionEndpoints& connection, NWON_COMMAND_TYPE		& requestOrResponse	, byte_t		* targetBuffer, uint32_t bufferSize);
	template<typename _tPOD>	inline	int32_t						dataSend										(::gdnet::SConnectionEndpoints& connection, const NWON_COMMAND_TYPE	requestOrResponse	, const _tPOD	& podSourceInstance)					{ return dataSend		(connection.Local, connection.Target, requestOrResponse, &podSourceInstance, sizeof(_tPOD)); }
	template<typename _tPOD>	inline	int32_t						dataReceive										(::gdnet::SConnectionEndpoints& connection, NWON_COMMAND_TYPE		& requestOrResponse	, _tPOD			& podTargetInstance)					{ return dataReceive	(connection.Local, connection.Target, requestOrResponse, &podTargetInstance, sizeof(_tPOD)); }

	static constexpr					const uint16_t				MAX_COMMAND_PAYLOAD_SIZE						= 504;
#pragma pack(push, 1)
	struct SCommandPacket {
											SHeaderUserData				PacketInfo										;
											uint8_t						Padding											;
											byte_t						Payload		[MAX_COMMAND_PAYLOAD_SIZE]			;
	};
	NWOL_POD(::gdnet, SCommandPacket, "Network command packet", "Stores a command and related data from a given packet."
		, ::nwol::GDATA_TYPE_UINT8
		, ::nwol::GDATA_TYPE_UINT16
		, ::nwol::GDATA_TYPE_UINT8
		, GTYPEID_MAKE_INT8(::gdnet::MAX_COMMAND_PAYLOAD_SIZE)
		);
#pragma pack(pop)
}

#endif  // NWON_COMMAND_H_9278340928374