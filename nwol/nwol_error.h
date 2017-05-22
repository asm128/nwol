#include "nwol_debug.h"
#include "nwol_debug.h"
#include "typeint.h"

#if defined (__WINDOWS__)
#include <string>
#endif

#ifndef __NWOL_ERROR_H__827394__
#define __NWOL_ERROR_H__827394__

namespace nwol
{
	typedef				int32_t			error_t;
	inline constexpr	bool			failed						(error_t errorCode)		noexcept	{ return 0 > errorCode; }
#if defined (__WINDOWS__)
						std::string		getWindowsErrorAsString		(uint64_t lastError);
#endif
} // namespace

#define errored(errVal)		(::nwol::failed(errVal)) 

#endif // __NWOL_ERROR_H__827394__
