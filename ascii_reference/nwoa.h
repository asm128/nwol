#include "application.h"

#ifndef __NWOA_H__983264902__
#define __NWOA_H__983264902__

struct SApplication	: public ::nwol::SApplicationBase {
							SApplication			(::nwol::SRuntimeValues	* runtimeValues)	: SApplicationBase(runtimeValues)	{
		GUI						= {{320, 200}, {200, 64}};
	}
};

int32_t					setup					(::SApplication	& instanceApp);
int32_t					update					(::SApplication	& instanceApp, bool exitRequested);
int32_t					render					(::SApplication	& instanceApp);
int32_t					cleanup					(::SApplication	& instanceApp);

#endif // __NWOA_H__983264902__
