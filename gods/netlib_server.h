#include "netlib.h"

#include "nwol_mutex.h"
#include "glist.h"

#ifndef __NETLIB_SERVER_H__9823409236498237469827364982734689237__
#define __NETLIB_SERVER_H__9823409236498237469827364982734689237__

#define MAX_CLIENTS_QUEUE 32

namespace nwol
{
	struct CClient {
		int32_t								m_id										= -1;
		::nwol::SConnectionEndpoint			* m_ClientListener							= 0;
		::nwol::SConnectionEndpoint			* m_ClientTarget							= 0;

		int32_t								InitClient									(::nwol::SConnectionEndpoint* listener, ::nwol::SConnectionEndpoint* target, int32_t id)	noexcept	{
			m_ClientListener					= listener;
			m_ClientTarget						= target;
			m_id								= id;
			return 0;
		}
	};	// struct
	GDECLARE_OBJ(CClient);

	struct CServer {
#if defined(__WINDOWS__)
		::nwol::refcount_t					QueuedConnectionCount						= 0;
#else
		::nwol::refcount_t					QueuedConnectionCount						;
#endif
		::nwol::SConnectionEndpoint			* ServerConnection							= 0;
		::nwol::SConnectionEndpoint			* QueuedConnectionList[MAX_CLIENTS_QUEUE]	= {};
		GLstObj(::nwol, CClient)			ClientConnections							= {};	
		GLstObj(::nwol, CClient)			ClientUnused								= {};	
		::nwol::CMutex						ConnectionsMutex							= {};
		char								HostName[256]								= {};	//
		bool								bListening									= false;	 
											
		::nwol::error_t						InitServer									(int32_t port_number);
		::nwol::error_t						Listen										();
		::nwol::error_t						Accept										();
		::nwol::error_t						ShutdownServer								();
		void								DisconnectClient							(int32_t id);

	};	// struct
}	// namespace

// Define this function for processing your commands.
int32_t executeCommand	(::nwol::CClient* client, const char* buffer);

#endif  // __NETLIB_SERVER_H__9823409236498237469827364982734689237__