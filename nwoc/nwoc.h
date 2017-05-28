#include "application.h"

#ifndef __NWOA_H__983264902__
#define __NWOA_H__983264902__

struct SApplicationDetail {	
#if defined(__WINDOWS__)
	WNDCLASSEX											WindowClass						= {};
	DWORD												WindowStyle						= WS_OVERLAPPED | WS_THICKFRAME | WS_BORDER | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;
#endif
};

struct SApplication : public ::nwol::SApplicationBase {
	::SApplicationDetail								PlatformDetail			= {};

	inline 												SApplication			(::nwol::SRuntimeValues	* runtimeValues)	: SApplicationBase(runtimeValues)	{
		GUI													= {runtimeValues ? runtimeValues->Screen.Metrics.Size.Cast<uint32_t>() : ::nwol::SCoord2<uint32_t>{320, 240}, ::nwol::SCoord2<uint32_t>{132, 60}};
	}
};

int32_t												setup			(::SApplication	& instanceApp);
int32_t												update			(::SApplication	& instanceApp, bool exitRequested);
int32_t												render			(::SApplication	& instanceApp);
int32_t												cleanup			(::SApplication	& instanceApp);
//																	 
::nwol::error_t										networkEnable	(::SApplication	& instanceApp);
::nwol::error_t										networkDisable	(::SApplication	& instanceApp);


#endif // __NWOA_H__983264902__
