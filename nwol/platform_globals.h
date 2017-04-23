#ifndef __PLATFORM_GLOBALS_H__2983423__
#define __PLATFORM_GLOBALS_H__2983423__

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
#define UNUSED __attribute__((__unused__))
#elif defined(__WINDOWS__)
#	ifdef _DEBUG
#		define UNUSED 
#	else
#		define UNUSED 
#	endif
#else
#define UNUSED
#endif

#if defined(__WINDOWS__)
#	pragma warning(disable : 4592)	// We need this because of a bug in the compiler causing our static const variables to throw a warning as if its constructor was defined as constexpr.
#endif

#if defined(__WINDOWS__)
#define DYNAMIC_LIBRARY_EXTENSION "dll"
#else
#define DYNAMIC_LIBRARY_EXTENSION "os"
#endif

#endif // __PLATFORM_GLOBALS_H__2983423__