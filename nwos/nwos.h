#include "application.h"

#include "netlib_server.h"

#ifndef __NWOA_H__983264902__
#define __NWOA_H__983264902__

struct SApplication	: public ::nwol::SApplicationBase {
	NWOM(::nwol, CServer					,	NetworkServer			, ::nwol::GDATA_TYPE_OBJ		, "Network server"				, "Holds server connection state and client information."		)	= {};	// 
	NWOM_REGISTRY
		(	NWOM_NAME(NetworkServer		)
		);
	inline										SApplication			(::nwol::SRuntimeValues	* runtimeValues)						: SApplicationBase(runtimeValues)	{ GUI = {{640, 480}, {132>>1, 60>>1}}; }
};

int32_t										setup					(::SApplication	& instanceApp);
int32_t										update					(::SApplication	& instanceApp, bool exitRequested);
int32_t										render					(::SApplication	& instanceApp);
int32_t										cleanup					(::SApplication	& instanceApp);
//																	 
::nwol::error_t								networkEnable			(::SApplication	& instanceApp);
::nwol::error_t								networkDisable			(::SApplication	& instanceApp);

#endif // __NWOA_H__983264902__
