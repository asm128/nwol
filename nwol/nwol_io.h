#include <cstdio>

#ifndef __NWOL_IO_H_56847984984__
#define __NWOL_IO_H_56847984984__

#if defined(ANDROID) || defined(__linux__)
	static inline int fopen_s(FILE** out_fp, const char* pFilename, const char* mode) {
		FILE* fp = fopen(pFilename, mode);
		*out_fp = fp;
		if( 0 == fp )
			return -1;
		else 
			return 0;
	}
#endif

	
#endif // __NWOL_IO_H_56847984984__
