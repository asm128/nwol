#include "nwol_io.h"
#include <cstring>

#ifndef __STRING_H_56847984984__
#define __STRING_H_56847984984__

#if defined(ANDROID) || defined(__linux__)

#	include <cstddef>
#	include <cstdarg>

	static inline							int	strcat_s		(char *dst, size_t bufferSize, const char *src)										{
		if((uint32_t)strlen(dst)+(uint32_t)strlen(src)+1U > (uint32_t)bufferSize)
			return -1;
		strcat(dst, src);
		return 0;
	}

	static inline							int	strcpy_s		(char *dst, size_t bufferSize, const char *src)										{
		if((uint32_t)strlen(src)+1U > (uint32_t)bufferSize)
			return -1;
		strcpy(dst, src);
		return 0;
	}

	static inline							int	_snprintf_s		(char* buffer, size_t bufferSize, size_t count, const char* format, ...)			{
		va_list args;
		va_start(args, format);
		const int										result			= vsnprintf( buffer, count, format, args );
		va_end(args);
		return result;
	}

	template<size_t _Size> static inline	int	_snprintf_s		(char (&buffer)[_Size], size_t count, const char* format, ...)						{
		va_list args;
		va_start(args, format);
		const int										result			= _snprintf_s( buffer, _Size, count, format, args );
		va_end(args);
		return result;
	}

	static inline							int	sprintf_s		(char *buffer, size_t bufferSize, const char *format, ...)							{
		va_list args;
		va_start(args, format);
		const int										result			= vsprintf(buffer, format, args);
		va_end(args);
		return result;
	}

	static inline							int	vsprintf_s		(char *buffer, size_t bufferSize, const char *format, ...)							{
		va_list args;
		va_start(args, format);
		const int										result			= vsprintf(buffer, format, args);
		va_end(args);
		return result;
	}

	template<size_t _Size> static inline	int	sprintf_s		(char (&buffer)[_Size], const char* format, ...)									{
		va_list args;
		va_start(args, format);
		const int										result			= vsprintf_s(buffer, _Size, format, args);
		va_end(args);
		return result;
	}

	template<size_t _Size> static inline	int	strcat_s		( char (&dst)[_Size], const char *src )												{ return strcat_s	(dst, _Size, src);				}
	template<size_t _Size> static inline	int	strcpy_s		( char (&dst)[_Size], const char *src )												{ return strcpy_s	(dst, _Size, src);				}
	static inline							int	_vsnprintf_s	( char* buffer, size_t bufferSize, size_t count, const char* format, va_list args )	{ return vsnprintf	(buffer, count, format, args);	}
	static inline							int	vsprintf_s		( char *buffer, size_t bufferSize, const char *format, va_list args )				{ return vsprintf	(buffer, format, args);			}
#else

#endif

#endif // __STRING_H_56847984984__
