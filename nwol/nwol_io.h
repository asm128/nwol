#include "platform_globals.h"
#include <cstdio>

#ifndef __NWOL_IO_H_56847984984__
#define __NWOL_IO_H_56847984984__

#if defined(__ANDROID__) || defined(__LINUX__)
	static inline int						fopen_s					(FILE** out_fp, const char* pFilename, const char* mode)				{
		reterr_error_if(0 == out_fp, "File pointer cannot be a null pointer.");
		FILE										* fp					= fopen(pFilename, mode);
		if(0 == (*out_fp = fp))
			return -1;
		else 
			return 0;
	}
#endif

#endif // __NWOL_IO_H_56847984984__
