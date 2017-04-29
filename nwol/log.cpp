#include "nwol_log.h"

#include "nwol_string.h"
#include <time.h>

#if defined(__ANDROID__) || defined(__LINUX__)
    #if defined(__ANDROID__)
        #include <android/log.h>
        #define GLOG(...) ((void)__android_log_print(__VA_ARGS__))
    #else // regular Linux
        #define GLOG(level, gods, format, ...) ((void)fprintf( stderr, format, __VA_ARGS__ ))
    #endif

    #define GLOGD(format, ...) GLOG(ANDROID_LOG_DEBUG, "GODS", format, __VA_ARGS__)
    #define GLOGI(format, ...) GLOG(ANDROID_LOG_INFO , "GODS", format, __VA_ARGS__)
    #define GLOGW(format, ...) GLOG(ANDROID_LOG_WARN , "GODS", format, __VA_ARGS__)
    #define GLOGE(format, ...) GLOG(ANDROID_LOG_ERROR, "GODS", format, __VA_ARGS__)
#elif defined(__WINDOWS__) // Windows
    #include <Windows.h>
#endif

void										nwol::__internal_debug_print_console			(const char* chars)						{ printf("%s", chars); }
void										nwol::__internal_debug_print_debugger			(const char* chars)						{
#if defined(__ANDROID__) || defined(__LINUX__)
	GLOGE( "%s", chars );
#elif defined(__WINDOWS__)
	//printf("%s", chars);
	OutputDebugStringA(chars);
#endif
}

void										nwol::__internal_debug_print_file				(const char* chars, int nCharCount )	{
//	_gods_internal_info_printf_debugger( chars );
//	return;
	static	char										__debug_file_name[48]						= {};												
	static	const time_t								__today_0									= time(0);		
#if defined(__ANDROID__) || defined(__LINUX__)
	static	tm											* __debug_tm								= gmtime(&__today_0);
	UNUSED static int									errMt_0										= _snprintf_s
		(	__debug_file_name
		,	sizeof(__debug_file_name)
		,	"log__%d__%d__%d.txt"
		,	__debug_tm ? __debug_tm->tm_year+1900		: 0
		,	__debug_tm ? __debug_tm->tm_mon+1			: 0
		,	__debug_tm ? __debug_tm->tm_mday			: 0
		); 
	GLOGE( "%s", chars );
	FILE												* __debug_fp								= fopen( __debug_file_name, "ab" );
	if( 0 == __debug_fp ) {
		if( (__debug_fp = fopen( __debug_file_name, "wb" )) ) {
			fwrite( chars, sizeof( char ), nCharCount, __debug_fp );
			fclose( __debug_fp );
		}
	}
	else { //if( ferrMy == 0 ) {
		fwrite( chars, sizeof( char ), nCharCount, __debug_fp );
		fclose( __debug_fp );
	}
#elif defined(__WINDOWS__)
	static tm											__debug_tm									= {};
	static const errno_t								__errMy										= gmtime_s(&__debug_tm, &__today_0);
	static const int									errMt_0										= _snprintf_s
		(	__debug_file_name, sizeof(__debug_file_name)
		,	"log__%d__%d__%d__%d__%d__%d.txt\0"
		,	__debug_tm.tm_year+1900
		,	__debug_tm.tm_mon+1
		,	__debug_tm.tm_mday
		,	__debug_tm.tm_hour
		,	__debug_tm.tm_min
		,	__debug_tm.tm_sec
		);
	FILE												* __debug_fp								= 0;
	OutputDebugStringA( chars );
	errno_t												ferrMy										= 0;
	if( 2 == (ferrMy = fopen_s( &__debug_fp, __debug_file_name, "ab" )) )	{
		if( (0 == fopen_s( &__debug_fp, __debug_file_name, "wb" )) && __debug_fp ) {
			fwrite( chars, sizeof( char ), nCharCount, __debug_fp );
			fclose( __debug_fp );
		}
	}
	else if( (0 == ferrMy) && __debug_fp ) 	{
		fwrite( chars, sizeof( char ), nCharCount, __debug_fp );
		fclose( __debug_fp );
	}
#endif 
}
