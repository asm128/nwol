#include "netlib_node.h"

#include "nwol_mutex.h"
#include "glist.h"

#ifndef __NETLIB_SERVER_H__9823409236498237469827364982734689237__
#define __NETLIB_SERVER_H__9823409236498237469827364982734689237__

#define MAX_CLIENTS_QUEUE 32

namespace gdnet
{
	struct SServer {
#if defined(__WINDOWS__)
					::nwol::refcount_t						QueuedConnectionCount										= 0;
#else
					::nwol::refcount_t						QueuedConnectionCount										;
#endif
					GPNCO(::gdnet, IEndpoint)				EndpointListener											= 0;
					GPNCO(::gdnet, IEndpoint)				QueuedConnectionList	[MAX_CLIENTS_QUEUE]					= {};
					GLstObj(::gdnet, SNetworkNode)			ClientConnections											= {};	
					::nwol::CMutex							ConnectionsMutex											= {};
					char									HostName[256]												= {};	//
					bool									bListening													= false;	 
															
					::nwol::error_t							InitServer													(uint16_t portLocal, ::gdnet::TRANSPORT_PROTOCOL mode = ::gdnet::TRANSPORT_PROTOCOL_UDP);
					::nwol::error_t							Listen														();
					::nwol::error_t							Accept														();
					::nwol::error_t							ShutdownServer												();
	};	// struct

				void									disconnectClient											(SServer& serverInstance, int32_t clientId);
}	// namespace

#endif  // __NETLIB_SERVER_H__9823409236498237469827364982734689237__