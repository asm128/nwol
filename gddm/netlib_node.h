#include "netlib_command.h"
#include "glist.h"

#ifndef NETLIB_NODE_H__90237423649827469283746928374698273
#define NETLIB_NODE_H__90237423649827469283746928374698273

namespace gdnet
{
#pragma pack(push, 1)
	struct				SNetworkNodeTimes	{	// This is called this way on purpose.
							uint64_t									TimeInSecondsRemote									= 0;
							uint64_t									TimeLastSent										= 0;
							uint64_t									TimeLastReceived									= 0;
							uint64_t									MicrosecondsLastPing								= 0;
	};

	struct				SNetworkNodeState	{
							bool										Connected											: 1;
							bool										Listening											: 1;
							bool										PausedPackets										: 1;
							bool										PausedConnections									: 1;
	};
#pragma pack(pop)

	struct				SNetworkNode		{
		typedef				::nwol::error_t								(*FCommandProcedure)								(const SNetworkNode& instanceNode);

							::gdnet::SConnectionEndpoints				Endpoints											= {};
							::gdnet::SConnectionAddress					Address												= {{}, {{192, 168, 1, 79}, 45678}};
							uint64_t									Id													= ~0ULL;
							::gdnet::SNetworkNodeTimes					Timestamps											= {};

							GLstPOD(::gdnet, SCommandPacket)			InputRequests										= {};
							GLstPOD(::gdnet, SCommandPacket)			InputResponses										= {};

							::nwol::array_view<FCommandProcedure>		SystemProcedureList									= {};

							::gdnet::SNetworkNodeState					State												= {};
	};
	GDECLARE_OBJ(SNetworkNode);

						::nwol::error_t								nodeInit											(::gdnet::SNetworkNode& instanceNode, const ::gdnet::SConnectionAddress& addresses, ::gdnet::TRANSPORT_PROTOCOL mode, uint64_t nodeId = ~0ULL);
						::nwol::error_t								nodeInit											(::gdnet::GODS(SNetworkNode) pnoderef, const ::gdnet::SConnectionEndpoints& connection, uint64_t id);
						::nwol::error_t								nodeConnect											(::gdnet::SNetworkNode& instanceNode);
						::nwol::error_t								nodeRequestDisconnect								(::gdnet::SNetworkNode& instanceNode);
} // namespace

// Define this function for processing your commands.
					int32_t										nodeHandleUserData										(::gdnet::SNetworkNode* client, const char* buffer, uint32_t bufferLen);

#endif // NETLIB_NODE_H__90237423649827469283746928374698273
