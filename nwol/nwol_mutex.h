/// Copyright 2015-2017 - asm128
#include "multithread.h"
#include "nwol_debug.h"

#if defined(__ANDROID__) || defined(__LINUX__)
#elif defined(__WINDOWS__)
#else
#error "Platform not supported."
#endif

#ifndef __NWOL_MUTEX_H_984165163546_ 
#define __NWOL_MUTEX_H_984165163546_

namespace nwol
{
	class CMutex {
		//friend	class			CLock;
		DECLARE_SHARED_SECTION	(CriticalSection);
		inline 									CMutex			(CMutex& other			)	= delete;
		inline	CMutex&							operator=		(CMutex& other			)	= delete;
	public:
		inline									CMutex			()							{ (void)(INIT_SHARED_SECTION	(CriticalSection)); }
		inline									~CMutex			()							{ DELETE_SHARED_SECTION			(CriticalSection);	}
#if defined(__WINDOWS__)
		_Acquires_lock_(this->CriticalSection)
#endif
		inline	void							Lock			()							{ ENTER_SHARED_SECTION			(CriticalSection);	}
#if defined(__WINDOWS__)
		_Releases_lock_(this->CriticalSection)
#endif
		inline	void							Unlock			()							{ LEAVE_SHARED_SECTION			(CriticalSection);	}
	};

	class CLock {
				CMutex							& mutex;

												CLock			()							= delete;
												CLock			(const CLock&)				= delete;
				CLock&							operator=		(const CLock&)				= delete;
	public:
		inline									~CLock			()							{ mutex.Unlock	(); }
		inline									CLock			(CMutex& m) : mutex(m)		{ mutex.Lock	(); }
	};
}

#endif // __NWOL_MUTEX_H_984165163546_
