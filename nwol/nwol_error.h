/// Copyright 2013-2017 - asm128
#include "typeint.h"

#if defined (__WINDOWS__)
#include <string>
#endif

#ifndef __NWOL_ERROR_H__827394__
#define __NWOL_ERROR_H__827394__

namespace nwol
{
	typedef						int32_t										error_t;
	static inline constexpr		bool										failed						(const error_t errorCode)		noexcept	{ return 0 >  errorCode;						}
	static inline constexpr		bool										succeeded					(const error_t errorCode)		noexcept	{ return 0 <= errorCode;						}
#if defined (__WINDOWS__)
								::std::string								getWindowsErrorAsString		(const uint64_t lastError);	// Get the error message, if any.
	static inline				::std::string								getOSErrorAsString			(const uint64_t lastError)					{ return getWindowsErrorAsString(lastError);	}
#else
	static inline				::std::string								getOSErrorAsString			(const uint64_t lastError)					{ return "";									}
#endif

} // namespace

#if !defined(errored)
#	define						errored(errVal)								(::nwol::failed(errVal)) 
#endif
#if !defined(not_errored)
#	define						not_errored(errVal)							(::nwol::succeeded(errVal)) 
#endif

#endif // __NWOL_ERROR_H__827394__
