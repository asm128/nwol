#include "member_registry.h"
#include "nwol_mutex.h"
#include "runtime.h"

#include "gui.h"
#include "nwol_input.h"
#include "netlib_client.h"
//#include "netlib_server.h"

#ifndef __NWOL_APPLICATION_H_98364902634__
#define __NWOL_APPLICATION_H_98364902634__

namespace nwol
{
	GDEFINE_ENUM_TYPE	(NETWORK_STATE, uint8_t);
	GDEFINE_ENUM_VALUE	(NETWORK_STATE, NONE	, 0);
	GDEFINE_ENUM_VALUE	(NETWORK_STATE, ENABLED	, 1);
	GDEFINE_ENUM_VALUE	(NETWORK_STATE, RUNNING	, 2);

	struct SApplicationNetworkClient {
		NWOM(::nwol, SClientConnection			,	Connection				, ::nwol::GDATA_TYPE_OBJ		, "Connection link"				, "Holds socket and connection information."					)	= {};
		NWOM(::nwol, CMutex						,	ServerTimeMutex			, ::nwol::GDATA_TYPE_OBJ		, "Server time mutex"			, "Locks access to server time variable."						)	= {};
		NWOM(, uint64_t							,	ServerTime				, ::nwol::GDATA_TYPE_UINT64		, "Server time"					, "Holds server time."											)	= 0;
		NWOM(::nwol, NETWORK_STATE				,	State					, ::nwol::GDATA_TYPE_UINT8		, "Network state bitfield"		, "Holds netowrk and connection states."						)	= NETWORK_STATE_NONE;
		NWOM_REGISTRY
			(	NWOM_NAME(Connection		)
			,	NWOM_NAME(ServerTimeMutex	)
			,	NWOM_NAME(ServerTime		)
			,	NWOM_NAME(State				)
			);
	};	// struct

	struct SApplicationBase {
		NWOM(::nwol, SRuntimeValues*			,	RuntimeValues			, ::nwol::GDATA_TYPE_POINTER	, "Runtime values"				, "Stores runtime states and loaded modules."					)	= 0;
		NWOM(::nwol, SInput						,	Input					, ::nwol::GDATA_TYPE_OBJ		, "Input subsystem"				, "Holds information of user input events and states."			)	= {};
		NWOM(::nwol, SGUI						,	GUI						, ::nwol::GDATA_TYPE_OBJ		, "Graphical user interface"	, "GUI control information is stored and drawn by this object."	)	= {{640, 480}, {132, 60},};
		NWOM(::nwol, SApplicationNetworkClient	,	NetworkClient			, ::nwol::GDATA_TYPE_OBJ		, "Network client"				, "Holds client connection state and server information."		)	= {};	// 
		NWOM_REGISTRY
			(	NWOM_NAME(RuntimeValues		)
			,	NWOM_NAME(Input				)
			,	NWOM_NAME(GUI				)
			,	NWOM_NAME(NetworkClient		)
			);

#if defined(__ANDROID__)
		inline										SApplicationBase		(::nwol::SRuntimeValues	* runtimeValues)						: RuntimeValues(runtimeValues)									{
			runtimeValues->AndroidAppState->userData	= this;					
			runtimeValues->AndroidAppState->onAppCmd	= SApplicationBase::handleAppCommand;	
		}
 		static	void								handleAppCommand		(android_app * androidAppState, int32_t androidCommand)			{}
#else
		inline										SApplicationBase		(::nwol::SRuntimeValues	* runtimeValues)						: RuntimeValues(runtimeValues)									{}
#endif
	};	// struct

}	// namespace

#endif // __NWOL_APPLICATION_H_98364902634__

