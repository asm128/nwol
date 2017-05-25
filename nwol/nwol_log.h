#include "nwol_size.h"
#include "nwol_string.h"
#include "nwol_error.h"

#if defined (__ANDROID__)
#	include <android\log.h>
#endif

#include <malloc.h>

#ifndef __LOG_H__9284087409823740923864192736__
#define __LOG_H__9284087409823740923864192736__

namespace nwol
{
			void							__internal_debug_print_file		( const char* chars, int nCharCount );
			void							__internal_debug_print_debugger	( const char* chars );
	inline	void							__internal_debug_print_console	( const char* chars )																{ printf("%s", chars); }
//#define FORCE_STD_PRINTF_DEBUG	
#if !(defined(NWOL_DEBUG_ENABLED))
//#define USE_FILE_DEBUG_PRINTF
#endif

#if defined( USE_FILE_DEBUG_PRINTF ) && !defined( FORCE_STD_PRINTF_DEBUG )
#	define _nwol_internal_info_printf( chars, nCharCount )		::nwol::__internal_debug_print_file( chars, nCharCount )
#	define _nwol_internal_debug_wprintf( chars, nCharCount )	OutputDebugStringW( chars )
#elif (defined(__WINDOWS__)) && !defined( FORCE_STD_PRINTF_DEBUG )
#	define _nwol_internal_info_printf( chars, nCharCount )		::nwol::__internal_debug_print_debugger( chars )
#	define _nwol_internal_debug_wprintf( chars, nCharCount )	OutputDebugStringW( chars )
#else // I use this because I don't have the debugger attached during release build test. Anyway it should be replaced with a proper fprintf to an open log stream.
#	define _nwol_internal_info_printf( chars, nCharCount )		::nwol::__internal_debug_print_console( chars )
#	define _nwol_internal_debug_wprintf( chars, nCharCount )	//wprintf( L"%s", chars )
#endif

	template<const size_t _sizePrefix, typename... TArgs>	
	void									__nwol_printf					(const char (&prefix)[_sizePrefix], const wchar_t* format, const TArgs... args)		{
		_nwol_internal_info_printf(prefix, _sizePrefix-1);
		wchar_t											customDynamicString	[4096]		= {0};
		const size_t									stringLength					= swprintf_s(customDynamicString, format, args...);
		_nwol_internal_debug_wprintf(customDynamicString, (int)stringLength);
		_nwol_internal_info_printf("\n", 1);
	}

	template<const size_t _sizePrefix, typename... TArgs>	
	void									__nwol_printf					(const char (&prefix)[_sizePrefix], const char* format, const TArgs... args)		{
		_nwol_internal_info_printf(prefix, _sizePrefix-1);
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

#define nwol_printf(severity, text, format, ...)																															\
	{																																										\
		static constexpr const char						infoFormat		[]								= ":" severity ":[" __FILE__ ":%i]|{" __FUNCTION__ "}:" text ":";	\
		static char										infoPrefix		[::nwol::size(infoFormat)+8]	= {};																\
		static const int								lengthPrefix									= sprintf_s(infoPrefix, infoFormat, __LINE__);						\
		::nwol::__nwol_printf(infoPrefix, format, __VA_ARGS__);																												\
	}
	
#define nwol_wprintf nwol_printf

#if defined( __ANDROID__ )
#	define always_printf( ... )										__android_log_print( ANDROID_LOG_DEBUG, __FILE__ ":", __VA_ARGS__ )
#else
#	define always_printf( format, ... )								nwol_printf(NWOL_ERROR_SEVERITY_DEBUG, "info", format, __VA_ARGS__)
#endif

#ifdef DEBUG_PRINTF_ENABLED
#	if defined( __ANDROID__ )
#		define info_printf( ... )										__android_log_print( ANDROID_LOG_DEBUG, __FILE__ ":", __VA_ARGS__ )
#	else
#		define info_printf( format, ... )								nwol_printf(NWOL_ERROR_SEVERITY_DEBUG, "info", format, __VA_ARGS__)
#	endif
#else
#	define info_printf( ... )										do {} while(false)
#endif

#ifdef WARNING_PRINTF_ENABLED
#	if defined( __ANDROID__ )
#		define warning_printf( ... )									__android_log_print(ANDROID_LOG_WARN, __FILE__ ":", __VA_ARGS__  )
#	elif defined( __WINDOWS__ )
#		define warning_printf( format, ... )							nwol_printf(NWOL_ERROR_SEVERITY_WARNING, "warning", format, __VA_ARGS__) 
#	else 
#		define warning_printf( format, ... )							nwol_printf(NWOL_ERROR_SEVERITY_WARNING, "warning", format, __VA_ARGS__ )
#	endif
#else
#	define warning_printf( ... )									do {} while(false)
#endif

#if defined(USE_DEBUG_BREAK_ON_ERROR_LOG)
#	define NWOL_PLATFORM_DEBUG_BREAK()								PLATFORM_CRT_BREAKPOINT()
#else
#	define NWOL_PLATFORM_DEBUG_BREAK()								do{}while(0)
#endif

#ifdef ERROR_PRINTF_ENABLED
#	if defined( __ANDROID__ )
#		define error_printf( ... )										__android_log_print(ANDROID_LOG_ERROR, __FILE__ ":", __VA_ARGS__  )
#	elif defined( __WINDOWS__ )
#		define error_printf( format, ... )								{ nwol_printf(NWOL_ERROR_SEVERITY_ERROR, "error", format, __VA_ARGS__ ); NWOL_PLATFORM_DEBUG_BREAK(); }
#	else
#		define error_printf( format, ... )								nwol_printf(NWOL_ERROR_SEVERITY_ERROR, "error", format, __VA_ARGS__ )
#	endif
#else
#	define error_printf( ... )										do {} while(false)
#endif

#ifdef DATA_PRINTF_ENABLED
#	if defined( __ANDROID__ )
#		define data_printf( ... )										__android_log_print(ANDROID_LOG_DEBUG, __FILE__ ":", __VA_ARGS__  )
#	else 
#		define data_printf( format, ... )								nwol_printf(NWOL_ERROR_SEVERITY_EXTENDED, "data", format, __VA_ARGS__ )
#	endif
#else
#	define data_printf( ... )										do {} while(false)
#endif

#ifdef VERBOSE_PRINTF_ENABLED
#	if defined( __ANDROID__ )
#		define verbose_printf( ... )									__android_log_print( ANDROID_LOG_DEBUG, __FILE__ ":", __VA_ARGS__ )
#	else
#		define verbose_printf( format, ... )							nwol_printf(NWOL_ERROR_SEVERITY_REALTIME, "verbose", format, __VA_ARGS__ )
#	endif
#else
#	define verbose_printf( ... )									do {} while(false)
#endif

#if !defined(NWOL_DONT_DEFINE_ERROR_PRINTF_ALIASES)

// These are meant for messages that don't require formatting.
#define error_print( message )									error_printf	("%s", message)
#define warning_print( message )								warning_printf	("%s", message)
#define debug_print( message )									info_printf		("%s", message)

#if defined(__ANDROID__)
#	define throw_if(condition, exception, ...)						if(condition) { nwol_printf(NWOL_ERROR_SEVERITY_ERROR	, "error"	, "condition: %s", #condition); error_printf(__VA_ARGS__); uint64_t * _tasdas = 0; for(uint32_t i = 0; i < 0xFFFFFFFF; ++i) _tasdas[i] = 0xFFFFFFFF00000000ULL; }	// No throw so we just crash.
#else
#	define throw_if(condition, exception, ...)						if(condition) { nwol_printf(NWOL_ERROR_SEVERITY_ERROR	, "error"	, "condition: %s", #condition); error_printf(__VA_ARGS__); throw(exception);	}
#endif
#define error_if(condition, ...)								if(condition) { nwol_printf(NWOL_ERROR_SEVERITY_ERROR	, "error"	, "condition: %s", #condition); error_printf	(__VA_ARGS__);					}
#define warn_if(condition, ...)									if(condition) { nwol_printf(NWOL_ERROR_SEVERITY_WARNING	, "warning"	, "condition: %s", #condition); warning_printf	(__VA_ARGS__);					}
#define info_if(condition, ...)									if(condition) { nwol_printf(NWOL_ERROR_SEVERITY_DEBUG	, "info"	, "condition: %s", #condition); info_printf		(__VA_ARGS__);					}

#define retval_error_if(retVal, condition, ...)					if(condition) { nwol_printf(NWOL_ERROR_SEVERITY_ERROR	, "error"	, "condition: %s", #condition); error_printf	(__VA_ARGS__); return retVal;	}
#define retval_warn_if( retVal, condition, ...)					if(condition) { nwol_printf(NWOL_ERROR_SEVERITY_WARNING	, "warning"	, "condition: %s", #condition); warning_printf	(__VA_ARGS__); return retVal;	}
#define retval_info_if( retVal, condition, ...)					if(condition) { nwol_printf(NWOL_ERROR_SEVERITY_DEBUG	, "info"	, "condition: %s", #condition); info_printf		(__VA_ARGS__); return retVal;	}

#define ret_error_if(condition, ...)							if(condition) { nwol_printf(NWOL_ERROR_SEVERITY_ERROR	, "error"	, "condition: %s", #condition); error_printf	(__VA_ARGS__); return;			}
#define ret_warn_if( condition, ...)							if(condition) { nwol_printf(NWOL_ERROR_SEVERITY_WARNING	, "warning"	, "condition: %s", #condition); warning_printf	(__VA_ARGS__); return;			}
#define ret_info_if( condition, ...)							if(condition) { nwol_printf(NWOL_ERROR_SEVERITY_DEBUG	, "info"	, "condition: %s", #condition); info_printf		(__VA_ARGS__); return;			}

#define break_error_if(condition, ...)							if(condition) { nwol_printf(NWOL_ERROR_SEVERITY_ERROR	, "error"	, "condition: %s", #condition); error_printf	(__VA_ARGS__); break;			}
#define break_warn_if( condition, ...)							if(condition) { nwol_printf(NWOL_ERROR_SEVERITY_WARNING	, "warning"	, "condition: %s", #condition); warning_printf	(__VA_ARGS__); break;			}
#define break_info_if( condition, ...)							if(condition) { nwol_printf(NWOL_ERROR_SEVERITY_DEBUG	, "info"	, "condition: %s", #condition); info_printf		(__VA_ARGS__); break;			}

#define reterr_error_if(condition, ...)							retval_error_if (-1, condition, __VA_ARGS__)
#define reterr_warn_if( condition, ...)							retval_warn_if	(-1, condition, __VA_ARGS__)
#define reterr_info_if( condition, ...)							retval_info_if	(-1, condition, __VA_ARGS__)

#define retnul_error_if(condition, ...)							retval_error_if ( 0, condition, __VA_ARGS__)
#define retnul_warn_if( condition, ...)							retval_warn_if	( 0, condition, __VA_ARGS__)
#define retnul_info_if( condition, ...)							retval_info_if	( 0, condition, __VA_ARGS__)

#define retwarn_error_if(condition, ...)						retval_error_if ( 1, condition, __VA_ARGS__)
#define retwarn_warn_if( condition, ...)						retval_warn_if	( 1, condition, __VA_ARGS__)
#define retwarn_info_if( condition, ...)						retval_info_if	( 1, condition, __VA_ARGS__)

#if defined(__WINDOWS__)
	#define error_last_system_error(sev, label)	nwol_printf(sev, label, "Last windows error: 0x%X '%s'", ::GetLastError(), ::nwol::getWindowsErrorAsString(::GetLastError()).c_str())
#else
	#define error_last_system_error(sev, label)	
#endif

// Propagable error call.
#define nwol_pecall(nwo_call, ...)																																				\
{	if(::nwol::error_t errCall = nwo_call) { 																																	\
		if(errCall < 0) {																																						\
			nwol_printf(NWOL_ERROR_SEVERITY_ERROR, "error", "%s: 0x%X", #nwo_call, errCall);																					\
			error_last_system_error(NWOL_ERROR_SEVERITY_ERROR, "error");																										\
			error_printf(__VA_ARGS__); 																																			\
			return -1; 																																							\
		}																																										\
}	}

// Propagable error-warning call.
#define nwol_pewcall(nwo_call, ...)																																				\
{	if(::nwol::error_t errCall = nwo_call) { 																																	\
		if(errCall < 0) {																																						\
			nwol_printf(NWOL_ERROR_SEVERITY_ERROR, "error", "%s: 0x%X", #nwo_call, errCall);																					\
			error_last_system_error(NWOL_ERROR_SEVERITY_ERROR, "error");																										\
			error_printf(__VA_ARGS__); 																																			\
			return -1; 																																							\
		}																																										\
		else {																																									\
			error_last_system_error(NWOL_ERROR_SEVERITY_WARNING, "warning");																									\
			warning_printf("%s: 0x%X", #nwo_call, errCall);																														\
		}																																										\
}	}


#define rve_if	retval_error_if
#define rvw_if	retval_warn_if
#define rvi_if	retval_info_if

#define re_if	ret_error_if
#define rw_if	ret_warn_if
#define ri_if	ret_info_if

#define ree_if	reterr_error_if
#define rew_if	reterr_warn_if
#define rei_if	reterr_info_if

#define rne_if	retnul_error_if
#define rnw_if	retnul_warn_if
#define rni_if	retnul_info_if

#define rwe_if	retwarn_error_if
#define rww_if	retwarn_warn_if
#define rwi_if	retwarn_info_if

#endif // !defined(NWOL_DONT_DEFINE_ERROR_PRINTF_ALIASES)

} // namespace

#endif //__LOG_H__9284087409823740923864192736__
