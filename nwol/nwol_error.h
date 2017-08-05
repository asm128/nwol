/// Copyright 2013-2017 - asm128
#include "typeint.h"

#include <string>

#ifndef NWOL_ERROR_H__827394__
#define NWOL_ERROR_H__827394__

namespace nwol
{
	typedef						int32_t										error_t;
	static inline constexpr		bool										failed						(const error_t errorCode)		noexcept	{ return 0 >  errorCode;						}
	static inline constexpr		bool										succeeded					(const error_t errorCode)		noexcept	{ return 0 <= errorCode;						}
#if defined (__WINDOWS__)
								::std::string								getWindowsErrorAsString		(const uint64_t lastError);	// Get the error message, if any.
	static inline				::std::string								getOSErrorAsString			(const uint64_t lastError)					{ return getWindowsErrorAsString(lastError);	}
#else
	static inline				::std::string								getOSErrorAsString			(const uint64_t lastError)					{ return "Unknown error";						}
#endif

} // namespace

#if !defined(errored)
#	define						errored(errVal)								(::nwol::failed(errVal)) 
#endif
#if !defined(not_errored)
#	define						not_errored(errVal)							(::nwol::succeeded(errVal)) 
#endif

#define NWOL_CRASH()		{ uint64_t * _tasdas = 0; for(uint32_t i = 0; i < 0xFFFFFFFF; ++i) _tasdas[i] = 0xFFFFFFFF00000000ULL; }	// No throw? Just crash.

#endif // NWOL_ERROR_H__827394__
