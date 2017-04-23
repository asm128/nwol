#include "application.h"

#ifndef __NWOA_H__983264902__
#define __NWOA_H__983264902__

struct SApplication : public ::nwol::SApplicationBase {
	inline 						SApplication	(::nwol::SRuntimeValues	* runtimeValues)	: SApplicationBase(runtimeValues)	{}
};

int32_t						setup			(::SApplication	& instanceApp);
int32_t						update			(::SApplication	& instanceApp, bool exitRequested);
int32_t						render			(::SApplication	& instanceApp);
int32_t						cleanup			(::SApplication	& instanceApp);
//											 
::nwol::error_t				networkEnable	(::SApplication	& instanceApp);
::nwol::error_t				networkDisable	(::SApplication	& instanceApp);


#endif // __NWOA_H__983264902__
