/// Copyright 2015-2017 - asm128
#include "nwol_sync.h"

#if defined(__ANDROID__) || defined(__LINUX__)
#elif defined(__WINDOWS__)
#else
#error "Platform not supported."
#endif

#ifndef NWOL_MUTEX_H_984165163546_ 
#define NWOL_MUTEX_H_984165163546_

namespace nwol
{
	class CMutex {
		//friend	class			CMutexGuard;
		DECLARE_SHARED_SECTION							(CriticalSection);
		inline 											CMutex						(CMutex& other)							= delete;
		inline	CMutex&									operator=					(CMutex& other)							= delete;
	public:
		inline											CMutex						()										{ (void)(INIT_SHARED_SECTION	(CriticalSection)); }
		inline											~CMutex						()										{ DELETE_SHARED_SECTION			(CriticalSection);	}
#if defined(__WINDOWS__)
		_Acquires_lock_(this->CriticalSection)
#endif
		inline	void									lock						()										{ ENTER_SHARED_SECTION			(CriticalSection);	}
#if defined(__WINDOWS__)
		_Releases_lock_(this->CriticalSection)
#endif
		inline	void									unlock						()										{ LEAVE_SHARED_SECTION			(CriticalSection);	}
	};

	class CMutexGuard {
				CMutex									& mutex;

														CMutexGuard					()										= delete;
														CMutexGuard					(const CMutexGuard&)					= delete;
				CMutexGuard&							operator=					(const CMutexGuard&)					= delete;
	public:
		inline											~CMutexGuard				()										{ mutex.unlock	(); }
		inline											CMutexGuard					(CMutex& m) : mutex(m)					{ mutex.lock	(); }
	};
}

#endif // NWOL_MUTEX_H_984165163546_
