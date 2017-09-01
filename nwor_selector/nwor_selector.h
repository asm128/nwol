#include "application.h"

#ifndef __NWOA_H__983264902__
#define __NWOA_H__983264902__

GDEFINE_ENUM_TYPE(SELECTOR_STATE, uint8_t);
GDEFINE_ENUM_VALUE(SELECTOR_STATE, START				, 0);
GDEFINE_ENUM_VALUE(SELECTOR_STATE, LOADING_APPS			, 1);
GDEFINE_ENUM_VALUE(SELECTOR_STATE, MENU					, 2);
GDEFINE_ENUM_VALUE(SELECTOR_STATE, LOADING_SELECTION	, 3);
GDEFINE_ENUM_VALUE(SELECTOR_STATE, RUNNING_SELECTION	, 4);
GDEFINE_ENUM_VALUE(SELECTOR_STATE, CLOSING				, 5);

struct SApplication : public ::nwol::SFramework {
	::nwol::refcount_t									RenderSemaphore					= 0;
	::SELECTOR_STATE									SelectorState					= SELECTOR_STATE_START;
	int32_t												ApplicationModuleSelected		= -1;
	::nwol::array_pod<::nwol::glabel>					ApplicationModulesName			= {};
	::nwol::array_pod<::nwol::SApplicationModule>		ApplicationModulesHandle		= {};

	inline												SApplication					(::nwol::SRuntimeValues	* runtimeValues)	: SFramework(runtimeValues)	{
		GUI													= {{640, 480}, {132, 25},};
	}
};

int32_t												setup							(::SApplication & instanceApp);
int32_t												update							(::SApplication & instanceApp, bool exitRequested);
int32_t												render							(::SApplication & instanceApp);
int32_t												cleanup							(::SApplication & instanceApp);

#endif // __NWOA_H__983264902__
