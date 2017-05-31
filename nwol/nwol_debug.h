/// Copyright 2010-2017 - asm128
#include "platform_globals.h"

#ifndef __NWOL_DEBUG_H__97826389432__
#define __NWOL_DEBUG_H__97826389432__

#if defined(NWOL_DEBUG_ENABLED)
#	if defined(__ANDROID__) || defined(__LINUX__)
#		define PLATFORM_CRT_BREAKPOINT()	do {} while(0)
#		define PLATFORM_CRT_CHECK_MEMORY()	do {} while(0)
#	elif defined(__WINDOWS__)
#		include <crtdbg.h>
#		define PLATFORM_CRT_BREAKPOINT		(void)_CrtDbgBreak
#		define PLATFORM_CRT_CHECK_MEMORY()	// (void)_CrtCheckMemory
#	else
#		error "Platform not supported."
#	endif
#	define ERROR_PRINTF_ENABLED	
#	define WARNING_PRINTF_ENABLED	
#	define DEBUG_PRINTF_ENABLED	
//#	define VERBOSE_PRINTF_ENABLED	// Uncomment as needed
//#	define DATA_PRINTF_ENABLED		// Uncomment as needed
#	ifndef DISABLE_DEBUG_BREAK_ON_ERROR_LOG
#		define USE_DEBUG_BREAK_ON_ERROR_LOG
#	endif
#else
#	define PLATFORM_CRT_BREAKPOINT()		do {} while(0)
#	define PLATFORM_CRT_CHECK_MEMORY()		do {} while(0)
#	define ERROR_PRINTF_ENABLED	
//#	define WARNING_PRINTF_ENABLED	// Uncomment as needed
//#	define DEBUG_PRINTF_ENABLED		// Uncomment as needed
//#	define VERBOSE_PRINTF_ENABLED	// Uncomment as needed
//#	define DATA_PRINTF_ENABLED		// Uncomment as needed
#endif

#endif // __NWOL_DEBUG_H__97826389432__
