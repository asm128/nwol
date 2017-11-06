/// Copyright 2010-2017 - asm128
#include "nwol_debug.h"

#ifndef NWOL_MULTITHREAD_H_098217309126309127836918723698127
#define NWOL_MULTITHREAD_H_098217309126309127836918723698127

#if defined(__ANDROID__) || defined(__LINUX__)
/// <Atomic Builtins> http://gcc.gnu.org/onlinedocs/gcc-4.4.3/gcc/Atomic-Builtins.html#Atomic-Builtins
#	include <mutex>
#	define sync_increment(nCount)							(__sync_add_and_fetch(&nCount, 1))
#	define sync_decrement(nCount)							(__sync_sub_and_fetch(&nCount, 1))
#	define sync_exchange(target, value)						(__sync_lock_test_and_set(&target, value))
#	define sync_compare_exchange(nCount, value, comparand)	(__sync_val_compare_and_swap(&nCount, comparand, value))
#	define DECLARE_SHARED_SECTION(Name)								::std::mutex Name //(true)
#	define INIT_SHARED_SECTION(Name)								(1)
#	define ENTER_SHARED_SECTION(Name)								(Name).lock()
#	define LEAVE_SHARED_SECTION(Name)								(Name).unlock()
#	define DELETE_SHARED_SECTION(Name)								(0)
#elif defined(__WINDOWS__)  
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	if (defined( _WIN64 ) || defined( WIN64 )) 
#		define sync_increment(nCount)							( InterlockedIncrement64		( &nCount ) )
#		define sync_decrement(nCount)							( InterlockedDecrement64		( &nCount ) )
#		define sync_exchange(nCount)							( InterlockedExchange64			( &nCount ) )
#		define sync_compare_exchange(nCount, value, comparand)	( InterlockedCompareExchange64	( &nCount, value, comparand ) )
#	elif (defined( _WIN32 ) || defined( WIN32 )) 
#		define sync_increment(nCount)							( InterlockedIncrement			( &nCount ) )
#		define sync_decrement(nCount)							( InterlockedDecrement			( &nCount ) )
#		define sync_exchange(nCount)							( InterlockedExchange			( &nCount ) )
#		define sync_compare_exchange(nCount, value, comparand)	( InterlockedCompareExchange	( &nCount, value, comparand ) )
#	endif
#	define DECLARE_SHARED_SECTION(Name)								CRITICAL_SECTION Name
#	define INIT_SHARED_SECTION(Name)								((int32_t)InitializeCriticalSectionAndSpinCount(&Name, 400))
#	define ENTER_SHARED_SECTION(Name)								EnterCriticalSection (&Name)
#	define LEAVE_SHARED_SECTION(Name)								LeaveCriticalSection (&Name)
#	define DELETE_SHARED_SECTION(Name)								DeleteCriticalSection(&Name)
#else
#	include <mutex>
#	define sync_increment(nCount)							(++(nCount))
#	define sync_decrement(nCount)							(--(nCount))
#	define sync_exchange(Target, Value)						(Target = Value)
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
#		define nwol_sync_increment				sync_increment
#		define nwol_sync_decrement				sync_decrement
#		define nwol_sync_exchange				sync_exchange 
#	elif defined(__WINDOWS__)  
#		if (defined( _WIN64 ) || defined( WIN64 )) 
#			define nwol_sync_increment(nCount)		( InterlockedIncrement64( &nCount ) )
#			define nwol_sync_decrement(nCount)		( InterlockedDecrement64( &nCount ) )
#			define nwol_sync_exchange(nCount)		( InterlockedExchange64 ( &nCount ) )
#		elif (defined( _WIN32 ) || defined( WIN32 )) 
#			define nwol_sync_increment				sync_increment
#			define nwol_sync_decrement				sync_decrement
#			define nwol_sync_exchange				sync_exchange 
#		endif
#	else
#		define nwol_sync_increment				sync_increment
#		define nwol_sync_decrement				sync_decrement
#		define nwol_sync_exchange				sync_exchange 
#	endif
#else
#	define nwol_sync_increment(nCount)		(++(nCount))
#	define nwol_sync_decrement(nCount)		(--(nCount))
#	define nwol_sync_exchange(Target, Value)	(Target = Value)

#	define DECLARE_CRITICAL_SECTION(...)	
#	define INIT_CRITICAL_SECTION(...)				(1)
#	define ENTER_CRITICAL_SECTION(...)				(0)
#	define LEAVE_CRITICAL_SECTION(...)				(0)
#	define DELETE_CRITICAL_SECTION(...)				do {} while(false)

#endif

#endif // NWOL_MULTITHREAD_H_098217309126309127836918723698127
