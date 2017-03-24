#include "platform_globals.h"

#ifndef __TYPEINT_H__941234698724987234__
#define __TYPEINT_H__941234698724987234__

#if defined( DISABLE_STDINT ) && (defined(WIN32) || defined(_WIN32))
typedef				__int8				int8_t; 	
typedef signed		__int8				sint8_t;	
typedef unsigned	__int8				uint8_t;	
typedef				__int16				int16_t;	
typedef signed		__int16				sint16_t;	
typedef unsigned	__int16				uint16_t;	
typedef				__int32				int32_t;	
typedef signed		__int32				sint32_t;	
typedef unsigned	__int32				uint32_t;	
typedef				__int64				int64_t;	
typedef signed		__int64				sint64_t;	
typedef unsigned	__int64				uint64_t;	
typedef				__int128			int128_t;	
typedef signed		__int128			sint128_t;	
typedef unsigned	__int128			uint128_t;	
#else
#	include <cstdint>	
#	include <cstddef>
#endif		
#if defined(__WINDOWS__)
typedef				float				float32_t;
typedef				double				float64_t;
typedef				unsigned char		byte_t;
typedef				char				char_t;
typedef				bool				bool_t;
#elif defined(__ANDROID__)
typedef				float				float32_t;
typedef				double				float64_t;
typedef				unsigned char		byte_t;
typedef				char				char_t;
typedef				bool				bool_t;
#elif defined(__LINUX__)
typedef				float				float32_t;
typedef				double				float64_t;
typedef				unsigned char		byte_t;
typedef				char				char_t;
typedef				bool				bool_t;
#else
#	error "byte_t not defined"
#endif
namespace nwol
{
#if defined(__ANDROID__) || defined(__LINUX__)
		typedef int					refcount_t;
#elif defined (__WINDOWS__)
#	if defined(_WIN64) || defined(WIN64)
		typedef volatile __int64	refcount_t;
#	else
		typedef volatile long		refcount_t;
#	endif
#else
#		include <atomic>
		typedef std::atomic<size_t> refcount_t;
#endif
		typedef const char*			cue_t;	// Special thanks to Kragen Javier Sitaker for this cue_t idea.
		typedef uint64_t			uid_t;
		typedef int32_t				id_t;

} // namespace

#endif // __TYPEINT_H__941234698724987234__
