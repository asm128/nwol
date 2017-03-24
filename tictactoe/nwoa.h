#include "application.h"

#include "TicTacToe.h"

#ifndef __NWOA_H__983264902__
#define __NWOA_H__983264902__

struct SApplication	: public ::nwol::SApplicationBase
{
	::ttt::TicTacToe			Game			= {};

								SApplication	(::nwol::SRuntimeValues	* runtimeValues)	: SApplicationBase(runtimeValues)	{
		GUI		= {{320, 200}, {40, 13},};	// customize GUI sizes
	}
};

int32_t			setup			(::SApplication	& instanceApp);
int32_t			update			(::SApplication	& instanceApp, bool exitRequested);
int32_t			render			(::SApplication	& instanceApp);
int32_t			cleanup			(::SApplication	& instanceApp);
//								 
::nwol::error_t	networkEnable	(::SApplication	& instanceApp);
::nwol::error_t	networkDisable	(::SApplication	& instanceApp);

#endif // __NWOA_H__983264902__
