#include "application.h"

#ifndef __NWOA_H__983264902__
#define __NWOA_H__983264902__

GDEFINE_ENUM_TYPE(SELECTOR_STATE, uint8_t);
GDEFINE_ENUM_VALUE(SELECTOR_STATE, 0, START				);
GDEFINE_ENUM_VALUE(SELECTOR_STATE, 1, LOADING_APPS		);
GDEFINE_ENUM_VALUE(SELECTOR_STATE, 2, MENU				);
GDEFINE_ENUM_VALUE(SELECTOR_STATE, 3, LOADING_SELECTION	);
GDEFINE_ENUM_VALUE(SELECTOR_STATE, 4, RUNNING_SELECTION	);
GDEFINE_ENUM_VALUE(SELECTOR_STATE, 5, CLOSING			);

struct SApplication : public ::nwol::SApplicationBase
{
	::nwol::refcount_t							RenderSemaphore				= 0;
	::SELECTOR_STATE							SelectorState				= SELECTOR_STATE_START;
	int32_t										ApplicationModuleSelected	= -1;
	::nwol::array_pod<::nwol::glabel>			ApplicationModulesName		= {};
	::nwol::array_pod<::nwol::SModuleInterface>	ApplicationModulesHandle	= {};

	inline										SApplication				(::nwol::SRuntimeValues	* runtimeValues)	: SApplicationBase(runtimeValues)	{
		GUI		= {{640, 480}, {132, 25},};
	}
};


int32_t setup	(::SApplication & instanceApp);
int32_t update	(::SApplication & instanceApp, bool exitRequested);
int32_t render	(::SApplication & instanceApp);
int32_t cleanup	(::SApplication & instanceApp);

#endif // __NWOA_H__983264902__
