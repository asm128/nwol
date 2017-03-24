#include "typeint.h"

#ifndef __NWOL_ERROR_H__827394__
#define __NWOL_ERROR_H__827394__

namespace nwol
{
	typedef				int32_t		error_t;
	inline constexpr	bool		failed(error_t errorCode)		{ return 0 > errorCode; }
} // namespace

#define errored(errVal)		(::nwol::failed(errVal)) 

#endif // __NWOL_ERROR_H__827394__
