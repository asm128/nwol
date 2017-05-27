#include "typeint.h"

#if defined (__WINDOWS__)
#include <string>
#endif

#ifndef __NWOL_ERROR_H__827394__
#define __NWOL_ERROR_H__827394__

namespace nwol
{
	typedef					int32_t										error_t;
	inline constexpr		bool										failed						(error_t errorCode)		noexcept	{ return 0 >  errorCode; }
	inline constexpr		bool										succeeded					(error_t errorCode)		noexcept	{ return 0 <= errorCode; }
#if defined (__WINDOWS__)
							::std::string								getWindowsErrorAsString		(uint64_t lastError);		// Get the error message, if any.
	static inline			::std::string								getOSErrorAsString			(uint64_t lastError)				{ return getWindowsErrorAsString(lastError);	}
#else
	static inline			::std::string								getOSErrorAsString			(uint64_t lastError)				{ return getWindowsErrorAsString(lastError);	}
#endif

} // namespace

#if !defined(errored)
#	define					errored(errVal)								(::nwol::failed(errVal)) 
#endif
#if !defined(not_errored)
#	define					not_errored(errVal)							(::nwol::succeeded(errVal)) 
#endif

#endif // __NWOL_ERROR_H__827394__
