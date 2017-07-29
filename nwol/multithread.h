/// Copyright 2010-2017 - asm128
#include "nwol_debug.h"

#ifndef NWOL_MULTITHREAD_H_098217309126309127836918723698127
#define NWOL_MULTITHREAD_H_098217309126309127836918723698127

#if defined(__ANDROID__) || defined(__LINUX__)
/// <Atomic Builtins> http://gcc.gnu.org/onlinedocs/gcc-4.4.3/gcc/Atomic-Builtins.html#Atomic-Builtins
#	include <mutex>
#	define INTERLOCKED_INCREMENT(nCount)							(__sync_add_and_fetch(&nCount, 1))
#	define INTERLOCKED_DECREMENT(nCount)							(__sync_sub_and_fetch(&nCount, 1))
#	define INTERLOCKED_EXCHANGE(target, value)						(__sync_lock_test_and_set(&target, value))
#	define INTERLOCKED_COMPARE_EXCHANGE(nCount, value, comparand)	(__sync_val_compare_and_swap(&nCount, comparand, value))
#	define DECLARE_SHARED_SECTION(Name)								::std::mutex Name //(true)
#	define INIT_SHARED_SECTION(Name)								(1)
#	define ENTER_SHARED_SECTION(Name)								(Name).lock()
#	define LEAVE_SHARED_SECTION(Name)								(Name).unlock()
#	define DELETE_SHARED_SECTION(Name)								(0)
#elif defined(__WINDOWS__)  
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	if (defined( _WIN64 ) || defined( WIN64 )) 
#		define INTERLOCKED_INCREMENT(nCount)							( InterlockedIncrement64		( &nCount ) )
#		define INTERLOCKED_DECREMENT(nCount)							( InterlockedDecrement64		( &nCount ) )
#		define INTERLOCKED_EXCHANGE(nCount)								( InterlockedExchange64			( &nCount ) )
#		define INTERLOCKED_COMPARE_EXCHANGE(nCount, value, comparand)	( InterlockedCompareExchange64	( &nCount, value, comparand ) )
#	elif (defined( _WIN32 ) || defined( WIN32 )) 
#		define INTERLOCKED_INCREMENT(nCount)							( InterlockedIncrement			( &nCount ) )
#		define INTERLOCKED_DECREMENT(nCount)							( InterlockedDecrement			( &nCount ) )
#		define INTERLOCKED_EXCHANGE(nCount)								( InterlockedExchange			( &nCount ) )
#		define INTERLOCKED_COMPARE_EXCHANGE(nCount, value, comparand)	( InterlockedCompareExchange	( &nCount, value, comparand ) )
#	endif
#	define DECLARE_SHARED_SECTION(Name)								CRITICAL_SECTION Name
#	define INIT_SHARED_SECTION(Name)								((int32_t)InitializeCriticalSectionAndSpinCount(&Name, 400))
#	define ENTER_SHARED_SECTION(Name)								EnterCriticalSection (&Name)
#	define LEAVE_SHARED_SECTION(Name)								LeaveCriticalSection (&Name)
#	define DELETE_SHARED_SECTION(Name)								DeleteCriticalSection(&Name)
#else
#	include <mutex>
#	define INTERLOCKED_INCREMENT(nCount)							(++(nCount))
#	define INTERLOCKED_DECREMENT(nCount)							(--(nCount))
#	define INTERLOCKED_EXCHANGE(Target, Value)						(Target = Value)
#	define DECLARE_SHARED_SECTION(Name)								::std::mutex Name //(true)
#	define INIT_SHARED_SECTION(Name)								(1)
#	define ENTER_SHARED_SECTION(Name)								(Name).lock()
#	define LEAVE_SHARED_SECTION(Name)								(Name).unlock()
#	define DELETE_SHARED_SECTION(Name)								(0)			
#endif

#if defined(NWOL_MTSUPPORT)
#	define DECLARE_CRITICAL_SECTION					DECLARE_SHARED_SECTION
#	define INIT_CRITICAL_SECTION					INIT_SHARED_SECTION
#	define ENTER_CRITICAL_SECTION					ENTER_SHARED_SECTION
#	define LEAVE_CRITICAL_SECTION					LEAVE_SHARED_SECTION
#	define DELETE_CRITICAL_SECTION					DELETE_SHARED_SECTION
#	if defined(__ANDROID__) || defined(__LINUX__)
#		define NWOL_INTERLOCKED_INCREMENT				INTERLOCKED_INCREMENT
#		define NWOL_INTERLOCKED_DECREMENT				INTERLOCKED_DECREMENT
#		define NWOL_INTERLOCKED_EXCHANGE				INTERLOCKED_EXCHANGE 
#	elif defined(__WINDOWS__)  
#		if (defined( _WIN64 ) || defined( WIN64 )) 
#			define NWOL_INTERLOCKED_INCREMENT(nCount)		( InterlockedIncrement64( &nCount ) )
#			define NWOL_INTERLOCKED_DECREMENT(nCount)		( InterlockedDecrement64( &nCount ) )
#			define NWOL_INTERLOCKED_EXCHANGE(nCount)		( InterlockedExchange64 ( &nCount ) )
#		elif (defined( _WIN32 ) || defined( WIN32 )) 
#			define NWOL_INTERLOCKED_INCREMENT				INTERLOCKED_INCREMENT
#			define NWOL_INTERLOCKED_DECREMENT				INTERLOCKED_DECREMENT
#			define NWOL_INTERLOCKED_EXCHANGE				INTERLOCKED_EXCHANGE 
#		endif
#	else
#		define NWOL_INTERLOCKED_INCREMENT				INTERLOCKED_INCREMENT
#		define NWOL_INTERLOCKED_DECREMENT				INTERLOCKED_DECREMENT
#		define NWOL_INTERLOCKED_EXCHANGE				INTERLOCKED_EXCHANGE 
#	endif
#else
#	define NWOL_INTERLOCKED_INCREMENT(nCount)		(++(nCount))
#	define NWOL_INTERLOCKED_DECREMENT(nCount)		(--(nCount))
#	define NWOL_INTERLOCKED_EXCHANGE(Target, Value)	(Target = Value)

#	define DECLARE_CRITICAL_SECTION(...)	
#	define INIT_CRITICAL_SECTION(...)				(1)
#	define ENTER_CRITICAL_SECTION(...)				(0)
#	define LEAVE_CRITICAL_SECTION(...)				(0)
#	define DELETE_CRITICAL_SECTION(...)				do {} while(false)

#endif

#endif // NWOL_MULTITHREAD_H_098217309126309127836918723698127
