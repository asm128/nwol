#ifndef __NWOL_DEBUG_H__97826389432__
#define __NWOL_DEBUG_H__97826389432__

#if defined(NWOL_DEBUG_ENABLED)
#
#	if defined(ANDROID) || defined(__linux__)
#		define PLATFORM_CRT_BREAKPOINT()	do{}while(0)
#		define PLATFORM_CRT_CHECK_MEMORY()	do{}while(0)
#	elif (defined( _WIN64 ) || defined( WIN64 ) || defined( _WIN32 ) || defined( WIN32 ))
#		include <crtdbg.h>
#		define PLATFORM_CRT_BREAKPOINT		(void)_CrtDbgBreak
#		define PLATFORM_CRT_CHECK_MEMORY()	//(void)_CrtCheckMemory 
#	else
#		error "Platform not supported."
#	endif
#
#	define ERROR_PRINTF_ENABLED	
#	define WARNING_PRINTF_ENABLED	
#	define DEBUG_PRINTF_ENABLED	
#	define RENDER_PRINTF_ENABLED	
#	define SOUND_PRINTF_ENABLED	
//#	ifndef DISABLE_DEBUG_BREAK_ON_ERROR_LOG
//#		define USE_DEBUG_BREAK_ON_ERROR_LOG
//#	endif
#else
#	define PLATFORM_CRT_BREAKPOINT()		do{}while(0)
#	define PLATFORM_CRT_CHECK_MEMORY()		do{}while(0)
#	define ERROR_PRINTF_ENABLED	
#	define WARNING_PRINTF_ENABLED	
#endif

#endif // __NWOL_DEBUG_H__97826389432__
