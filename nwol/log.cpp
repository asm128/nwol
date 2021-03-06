/// Copyright 2011-2017 - asm128
#include "nwol_log.h"
#include "nwol_safe.h"
#include "nwol_string.h"
#include <time.h>

#if defined(__ANDROID__) || defined(__LINUX__)
#	if defined(__ANDROID__)
#		include <android/log.h>
#		define GLOG(...) ((void)__android_log_print(__VA_ARGS__))
#	else // regular Linux
#		define GLOG(level, gods, format, ...) ((void)fprintf( stderr, format, __VA_ARGS__ ))
#	endif
#	define GLOGD(format, ...)					GLOG(ANDROID_LOG_DEBUG, "GODS", format, __VA_ARGS__)
#	define GLOGI(format, ...)					GLOG(ANDROID_LOG_INFO , "GODS", format, __VA_ARGS__)
#	define GLOGW(format, ...)					GLOG(ANDROID_LOG_WARN , "GODS", format, __VA_ARGS__)
#	define GLOGE(format, ...)					GLOG(ANDROID_LOG_ERROR, "GODS", format, __VA_ARGS__)
#elif defined(__WINDOWS__) // Windows
#	include <Windows.h>
#endif

void										nwol::_internal_debug_print_debugger			(const char* chars)								{
#if defined(__ANDROID__) || defined(__LINUX__)
	GLOGE( "%s", chars );
#elif defined(__WINDOWS__)
	//printf("%s", chars);
	OutputDebugStringA(chars);
#endif
}

void										nwol::_internal_debug_print_file				(const char* chars, int nCharCount )			{
	static	char										__debug_file_name[48]						= {};												
	static	const time_t								__today_0									= time(0);		
	static tm											__debug_tm									= {};
	static const errno_t								__errMy										= gmtime_s(&__debug_tm, &__today_0);
	static const int									errMt_0										= _snprintf_s
		(	__debug_file_name, sizeof(__debug_file_name)
		,	"log__%d__%d__%d__%d__%d__%d.txt\0"
		,	__debug_tm.tm_year	+ 1900
		,	__debug_tm.tm_mon	+ 1
		,	__debug_tm.tm_mday
		,	__debug_tm.tm_hour
		,	__debug_tm.tm_min
		,	__debug_tm.tm_sec
		);
	FILE												* __debug_fp								= 0;
	_internal_debug_print_debugger( chars );
	errno_t												ferrMy										= 0;
	if( 2 == (ferrMy = fopen_s( &__debug_fp, __debug_file_name, "ab" )) )	{
		if( (0 == fopen_s( &__debug_fp, __debug_file_name, "wb" )) && __debug_fp )
			fwrite( chars, sizeof( char ), nCharCount, __debug_fp );
	}
	else if( (0 == ferrMy) && __debug_fp ) {
		fwrite( chars, sizeof( char ), nCharCount, __debug_fp );
	}
	safe_fclose( __debug_fp );
}

void										nwol::_nwol_print_system_errors					(const char* prefix)							{
	char											systemErrorString	[4096]						= {0};
	char											resultString		[4096]						= {0};
	int												lastSystemError									= errno;
	if(lastSystemError) {
		::strerror_s(systemErrorString, lastSystemError);
		_nwol_internal_info_printf(prefix, _sizePrefix-1);
		size_t											stringLength									= ::sprintf_s(resultString, "Last system error: 0x%X '%s'", errno, systemErrorString);
		(void)stringLength;
		_nwol_internal_info_printf(resultString, stringLength);
		_nwol_internal_info_printf("\n", 1);
	}
#if defined(__WINDOWS__)
	DWORD											lastWindowsError								= ::GetLastError();
	if(lastWindowsError) {
		_nwol_internal_info_printf(prefix, _sizePrefix-1);
		size_t											stringLength									= ::sprintf_s(resultString, "Last Windows error: 0x%X '%s'", lastWindowsError, ::nwol::getWindowsErrorAsString(lastWindowsError).c_str());
		(void)stringLength;
		_nwol_internal_info_printf(resultString, stringLength);
		_nwol_internal_info_printf("\n", 1);
		::SetLastError(0);
	}
#endif

}
