#ifndef __PLATFORM_GLOBALS_H__2983423__
#define __PLATFORM_GLOBALS_H__2983423__

#if (defined(DEBUG) || defined(_DEBUG)) && (!defined(NWOL_DISABLE_DEBUG)) && !defined(NWOL_DEBUG_ENABLED) 
#	define NWOL_DEBUG_ENABLED
#	if !defined(USE_DEBUG_BREAK_ON_ERROR_LOG)
#		define USE_DEBUG_BREAK_ON_ERROR_LOG
#	endif
#endif	// _DEBUG

#if (!defined(NWOL_MTSUPPORT)) && !defined(NWOL_DISABLE_MTSUPPORT)
#	define NWOL_MTSUPPORT
#endif

#if (!defined(__ANDROID__)) && defined(ANDROID) 
#	define __ANDROID__
#elif (!defined(__LINUX__)) && defined(__linux__)
#	define __LINUX__
#elif (!defined(__WINDOWS__)) && (defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64))
#	define __WINDOWS__
#else
#	error "Unsupported platform."
#	define __UNKNOWN__
#endif

#ifdef __GNUC__
#	define UNUSED __attribute__((__unused__))
#elif defined(__WINDOWS__)
#	ifdef _DEBUG
#		define UNUSED 
#	else
#		define UNUSED 
#	endif
#else
#	define UNUSED
#endif

#if defined(__WINDOWS__)
//#	pragma warning(disable : 4592)	// We needed this because of a bug in the compiler causing our static const variables to throw a warning as if its constructor was defined as constexpr.
#	pragma warning(disable : 6326)	// Bogus potential comparison of a constant with another constant for static template functionality.
#	pragma warning(disable : 4706)	// Enable assignment within conditional expression
// GSL / Guide for Super Lammers
#	pragma warning(disable : 26461)	// Enable not marking as const
#	pragma warning(disable : 26481)	// Enable pointer arithmetic
#	pragma warning(disable : 26482)	// Enable indexing arrays without constexpr x.X suck a dick souprot & sucker
#	pragma warning(disable : 26485)	// Enable no pointer decay
#	pragma warning(disable : 26493)	// Enable C-style casts
#	pragma warning(disable : 26495)	// Not always initialize a member variable
#	pragma warning(disable : 26496)	// Enable non-const for() boundaries
#endif

#if defined(__WINDOWS__)
#	if !defined(NOMINMAX)
#		define NOMINMAX
#	endif 
#	if !defined(WIN32_LEAN_AND_MEAN)
#		define WIN32_LEAN_AND_MEAN
#	endif
#	define DYNAMIC_LIBRARY_EXTENSION "dll"
#else
#	define DYNAMIC_LIBRARY_EXTENSION "os"
#endif

#endif // __PLATFORM_GLOBALS_H__2983423__