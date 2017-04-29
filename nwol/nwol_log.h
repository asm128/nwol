#include "build_global.h"
#include "nwol_io.h"
#include "nwol_size.h"
#include "nwol_string.h"


#if defined (__ANDROID__)
#include <android\log.h>
#endif

#include <malloc.h>

#ifndef __LOG_H__9284087409823740923864192736__
#define __LOG_H__9284087409823740923864192736__

namespace nwol
{
	void __internal_debug_print_file		( const char* chars, int nCharCount );
	void __internal_debug_print_debugger	( const char* chars );
	void __internal_debug_print_console		( const char* chars );

//#define FORCE_STD_PRINTF_DEBUG	

#if !(defined(NWOL_DEBUG_ENABLED))
//#define USE_FILE_DEBUG_PRINTF
#endif

#if defined( USE_FILE_DEBUG_PRINTF ) && !defined( FORCE_STD_PRINTF_DEBUG )
#define _nwol_internal_info_printf( chars, nCharCount ) ::nwol::__internal_debug_print_file( chars, nCharCount )
#define _nwol_internal_debug_wprintf( chars, nCharCount )  OutputDebugStringW( chars )
#elif (defined(__WINDOWS__)) && !defined( FORCE_STD_PRINTF_DEBUG )
#define _nwol_internal_info_printf( chars, nCharCount ) ::nwol::__internal_debug_print_debugger( chars )
#define _nwol_internal_debug_wprintf( chars, nCharCount )  OutputDebugStringW( chars )
#else // I use this because I don't have the debugger attached during release build test. Anyway it should be replaced with a proper fprintf to an open log stream.
#define _nwol_internal_info_printf( chars, nCharCount ) ::nwol::__internal_debug_print_console( chars )
#define _nwol_internal_debug_wprintf( chars, nCharCount ) //wprintf( L"%s", chars )
#endif

	template<typename... TArgs>	void			__nwol_printf					(const char* prefix, int /*prefixSize*/, const wchar_t* format, const TArgs... args){
		_nwol_internal_info_printf(prefix, prefixSize);
		wchar_t											customDynamicString	[4096]		= {0};
		const size_t									stringLength					= swprintf_s(customDynamicString, format, args...);
		_nwol_internal_debug_wprintf(customDynamicString, (int)stringLength);
		_nwol_internal_info_printf("\n", 1);
	}

	template<typename... TArgs>	void			__nwol_printf					(const char* prefix, int /*prefixSize*/, const char* format, const TArgs... args)	{
		_nwol_internal_info_printf(prefix, prefixSize);
		char											customDynamicString	[4096]		= {0};
		const size_t									stringLength					= sprintf_s(customDynamicString, format, args...);
		_nwol_internal_info_printf(customDynamicString, (int)stringLength);
		_nwol_internal_info_printf("\n", 1);
	}

#define NWOL_ERROR_SEVERITY_REALTIME	"0"	// I love feedback.
#define NWOL_ERROR_SEVERITY_EXTENDED	"1"	
#define NWOL_ERROR_SEVERITY_DEBUG		"2"	
#define NWOL_ERROR_SEVERITY_WARNING		"3"	
#define NWOL_ERROR_SEVERITY_ERROR		"4"	
#define NWOL_ERROR_SEVERITY_FATAL		"5"	// Red Alert.

#define nwol_printf(severity, text, format, ...)																													\
	{																																								\
		static constexpr const char				infoFormat		[]								= ":" severity ":[" __FILE__ ":%i]{" __FUNCTION__ "}:" text ":";	\
		static char								infoPrefix		[::nwol::size(infoFormat)+8]	= {};																\
		static const int						lengthPrefix									= sprintf_s(infoPrefix, infoFormat, __LINE__);						\
		::nwol::__nwol_printf(infoPrefix, lengthPrefix, format, __VA_ARGS__);																						\
	}
	
#define nwol_wprintf nwol_printf

#if defined( __ANDROID__ )
#	define always_printf( ... )					__android_log_print( ANDROID_LOG_DEBUG, __FILE__ ":", __VA_ARGS__ )
#else
#	define always_printf( format, ... )			nwol_printf(NWOL_ERROR_SEVERITY_DEBUG, "info", format, __VA_ARGS__)
#endif


#ifdef DEBUG_PRINTF_ENABLED
#	if defined( __ANDROID__ )
#		define info_printf( ... )					__android_log_print( ANDROID_LOG_DEBUG, __FILE__ ":", __VA_ARGS__ )
#	else
#		define info_printf( format, ... )			nwol_printf(NWOL_ERROR_SEVERITY_DEBUG, "info", format, __VA_ARGS__)
#	endif
#else
#	define info_printf( ... ) do {} while(false)
#endif

#ifdef WARNING_PRINTF_ENABLED
#	if defined( __ANDROID__ )
#		define warning_printf( ... )				__android_log_print(ANDROID_LOG_WARN, __FILE__ ":", __VA_ARGS__  )
#	elif defined( __WINDOWS__ )
#		define warning_printf( format, ... )		nwol_printf(NWOL_ERROR_SEVERITY_WARNING, "warning", format, __VA_ARGS__) 
#	else 
#		define warning_printf( format, ... )		nwol_printf(NWOL_ERROR_SEVERITY_WARNING, "warning", format, __VA_ARGS__ )
#	endif
#else
#	define warning_printf( ... ) do {} while(false)
#endif

#if defined(USE_DEBUG_BREAK_ON_ERROR_LOG)
#define NWOL_PLATFORM_DEBUG_BREAK()	PLATFORM_CRT_BREAKPOINT()
#else
#define NWOL_PLATFORM_DEBUG_BREAK()	do{}while(0)
#endif

#ifdef ERROR_PRINTF_ENABLED
#	if defined( __ANDROID__ )
#		define error_printf( ... )					__android_log_print(ANDROID_LOG_ERROR, __FILE__ ":", __VA_ARGS__  )
#	elif defined( __WINDOWS__ )
#		define error_printf( format, ... )	{		nwol_printf(NWOL_ERROR_SEVERITY_ERROR, "error", format, __VA_ARGS__ ); NWOL_PLATFORM_DEBUG_BREAK(); }
#	else
#		define error_printf( format, ... )			nwol_printf(NWOL_ERROR_SEVERITY_ERROR, "error", format, __VA_ARGS__ )
#	endif
#else
#	define error_printf( ... ) do {} while(false)
#endif

#ifdef DATA_PRINTF_ENABLED
#	if defined( __ANDROID__ )
#		define data_printf( ... )					__android_log_print(ANDROID_LOG_DEBUG, __FILE__ ":", __VA_ARGS__  )
#	else 
#		define data_printf( format, ... )			nwol_printf(NWOL_ERROR_SEVERITY_EXTENDED, "data", format, __VA_ARGS__ )
#	endif
#else
#	define data_printf( ... ) do {} while(false)
#endif

#ifdef RENDER_PRINTF_ENABLED
#	if defined( __ANDROID__ )
#		define render_printf( ... )					__android_log_print(ANDROID_LOG_DEBUG, __FILE__ ":", __VA_ARGS__  )
#	else
#		define render_printf( format, ... )			nwol_printf(NWOL_ERROR_SEVERITY_REALTIME, "render", format, __VA_ARGS__ )
#	endif
#else
#	define render_printf( ... ) do {} while(false)
#endif

#ifdef SOUND_PRINTF_ENABLED
#	if defined( __ANDROID__ )
#		define sound_printf( ... )					__android_log_print(ANDROID_LOG_DEBUG, __FILE__ ":", __VA_ARGS__  )
#	else
#		define sound_printf( format, ... )			nwol_printf(NWOL_ERROR_SEVERITY_DEBUG, "sound", format, __VA_ARGS__ )
#	endif
#else
#	define sound_printf( ... ) do {} while(false)
#endif

#ifdef VERBOSE_PRINTF_ENABLED
#	if defined( __ANDROID__ )
#		define verbose_printf( ... )				__android_log_print( ANDROID_LOG_DEBUG, __FILE__ ":", __VA_ARGS__ )
#	else
#		define verbose_printf( format, ... )		nwol_printf(NWOL_ERROR_SEVERITY_REALTIME, "verbose", format, __VA_ARGS__ )
#	endif
#else
#	define verbose_printf( ... ) do {} while(false)
#endif

// These are meant for messages that don't require formatting.
#define error_print( message )						error_printf	("%s", message)
#define warning_print( message )					warning_printf	("%s", message)
#define debug_print( message )						info_printf		("%s", message)

#if defined(__ANDROID__)
#define throw_if(condition, exception, ...)			if(condition) { error_printf	(__VA_ARGS__); char* _tasdas = 0; *_tasdas = 123; }
#else
#define throw_if(condition, exception, ...)			if(condition) { error_printf	(__VA_ARGS__); throw(exception); }
#endif
#define error_if(condition, ...)					if(condition) { error_printf	(__VA_ARGS__); }
#define warn_if(condition, ...)						if(condition) { warning_printf	(__VA_ARGS__); }
#define info_if(condition, ...)						if(condition) { info_printf		(__VA_ARGS__); }

#define retval_error_if(retVal, condition, ...)		if(condition) { error_printf	(__VA_ARGS__); return retVal; }
#define retval_warn_if( retVal, condition, ...)		if(condition) { warning_printf	(__VA_ARGS__); return retVal; }
#define retval_info_if( retVal, condition, ...)		if(condition) { info_printf		(__VA_ARGS__); return retVal; }

#define reterr_error_if(condition, ...)				retval_error_if (-1, condition, __VA_ARGS__)
#define reterr_warn_if( condition, ...)				retval_warn_if	(-1, condition, __VA_ARGS__)
#define reterr_info_if( condition, ...)				retval_info_if	(-1, condition, __VA_ARGS__)

#define retnul_error_if(condition, ...)				retval_error_if ( 0, condition, __VA_ARGS__)
#define retnul_warn_if( condition, ...)				retval_warn_if	( 0, condition, __VA_ARGS__)
#define retnul_info_if( condition, ...)				retval_info_if	( 0, condition, __VA_ARGS__)

#define retwarn_error_if(condition, ...)			retval_error_if ( 1, condition, __VA_ARGS__)
#define retwarn_warn_if( condition, ...)			retval_warn_if	( 1, condition, __VA_ARGS__)
#define retwarn_info_if( condition, ...)			retval_info_if	( 1, condition, __VA_ARGS__)

#define retval_error_if_errored(retVal, errorCode, format, ...)	retval_error_if( retVal, errored(errorCode), "Error code: 0x%X. " format, errorCode, __VA_ARGS__)
#define retwarn_error_if_errored(errorCode, format, ...)		retval_error_if( 1, errorCode, format, __VA_ARGS__)
#define retnul_error_if_errored( errorCode, format, ...)		retval_error_if( 0, errorCode, format, __VA_ARGS__)
#define reterr_error_if_errored( errorCode, format, ...)		retval_error_if(-1, errorCode, format, __VA_ARGS__)

} // namespace

#endif //__LOG_H__9284087409823740923864192736__
