#include "application.h"
#include "nwol_screen.h"

#ifndef __NWOA_H__983264902__
#define __NWOA_H__983264902__

struct SApplicationDetail {
#if defined(__WINDOWS__)
	WNDCLASSEX					MainWindowClass			= {};
	DWORD						MainWindowStyle			= WS_OVERLAPPED | WS_THICKFRAME | WS_BORDER | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;
#endif
};

struct SApplication : public ::nwol::SApplicationBase {
	SApplicationDetail			PlatformDetail			= {};
	::nwol::SScreen				Screen					= {};

	inline 						SApplication			(::nwol::SRuntimeValues	* runtimeValues)	: SApplicationBase(runtimeValues)	{
		Screen.Metrics				= {{10, 10}, {320, 240}};
		GUI							= {Screen.Metrics.Size.Cast<uint32_t>(), {132, 60},};
	}
};

int32_t						setup			(::SApplication	& instanceApp);
int32_t						update			(::SApplication	& instanceApp, bool exitRequested);
int32_t						render			(::SApplication	& instanceApp);
int32_t						cleanup			(::SApplication	& instanceApp);
//											 
::nwol::error_t				networkEnable	(::SApplication	& instanceApp);
::nwol::error_t				networkDisable	(::SApplication	& instanceApp);


#endif // __NWOA_H__983264902__
