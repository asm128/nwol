#include "nwol_platform_globals.h"
#include "nwol_error.h"
#if defined(__WINDOWS__)
#define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#else
#	include <thread>
#endif

#ifndef NWOL_SLEEP_H_9027340928374
#define NWOL_SLEEP_H_9027340928374

namespace nwol
{
#if defined(__WINDOWS__)
	static inline	::nwol::error_t			sleep				(uint32_t milliseconds)				{ Sleep((uint32_t)milliseconds); return 0; }
#else
	static inline	::nwol::error_t			sleep				(uint32_t milliseconds)				{ ::std::this_thread::sleep_for(::std::chrono::milliseconds(milliseconds)); return 0; }
#endif

} // namespace

#endif // NWOL_SLEEP_H_9027340928374
