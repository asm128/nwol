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
	public:
								CMutex			()							{ (void)(INIT_SHARED_SECTION	(CriticalSection)); }
								~CMutex			()							{ DELETE_SHARED_SECTION			(CriticalSection);	}
#if defined(__WINDOWS__)
		_Acquires_lock_(this->CriticalSection)
#endif
				void			Lock			()							{ ENTER_SHARED_SECTION			(CriticalSection);	}
#if defined(__WINDOWS__)
		_Releases_lock_(this->CriticalSection)
#endif
				void			Unlock			()							{ LEAVE_SHARED_SECTION			(CriticalSection);	}
	};

	class CLock {
				CMutex			& mutex;

								CLock			()							= delete;
								CLock			(const CLock&)				= delete;
				CLock&			operator=		(const CLock&)				= delete;
	public:
		inline					~CLock			()							{ mutex.Unlock	(); }
		inline					CLock			(CMutex& m) : mutex(m)		{ mutex.Lock	(); }
	};
}

#endif // __NWOL_MUTEX_H_984165163546_
