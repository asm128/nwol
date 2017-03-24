
#ifndef __GREF_DEFINITION_COMMON_H_293874922__
#define __GREF_DEFINITION_COMMON_H_293874922__

#if defined(ERROR_PRINTF_ENABLED)	// otherwise error_printf() has no effect and these macros resolve into nothing. Removing them reduces warnings from the compiler because of the unused string array.
static const char* __string_table_errors[7]= {
											"Buffer overrun detected!",
											"Reference counter is zero! This is often because of a gptr_pod being cast to a GODS() type and then released with grelease(). As casting to a reference to GODS doesn't mean the pointer is being copied, the reference counter is not incremented.",
											"Heap corruption detected!",
											"Failed to allocate! Out of memory?",	
											"Failed to read from file!",
											"Failed to write to file!"				
										};		

#define errmsg_buffover()			error_printf("%s", __string_table_errors[0])
#define errmsg_refcountnull()		error_printf("%s", __string_table_errors[1])
#define errmsg_heapcorrupt()		error_printf("%s", __string_table_errors[2])
#define errmsg_alloc()				error_printf("%s", __string_table_errors[3])
#define errmsg_fileread()			error_printf("%s", __string_table_errors[4])
#define errmsg_filewrite()			error_printf("%s", __string_table_errors[5])
#else
#define errmsg_buffover()			
#define errmsg_refcountnull()		
#define errmsg_heapcorrupt()		
#define errmsg_alloc()				
#define errmsg_fileread()			
#define errmsg_filewrite()			
#endif

#define __GDEFINE_COPY(baseType)			void			gcopy	( GREF(baseType)* dst, const GREF(baseType)* src )	{ __g_##baseType##Manager.copyInstance(dst, src ? src->get() : 0);	}
#define __GDEFINE_CREATECUSTOM(baseType)	void			gcreate	( GREF(baseType)** p1, const baseType& src )		{ __g_##baseType##Manager.createRef(p1, src);						}

//--------------------------------------------------------------
#define __GDEFINE_RELEASE( baseType )							\
	void grelease( GREF(baseType)** _p ) {						\
		if( 0 == (*_p) )										\
			return;												\
		typedef void(*funcType)(GREF(baseType)**);				\
		if( (*_p)->Globals->__prelease != (funcType)grelease )	\
			((funcType)(*_p)->Globals->__prelease)(_p);			\
		else													\
			__g_##baseType##Manager.releaseRef(_p);				\
	}

#define __GDEFINE_CLONE( baseType )										\
	void gclone( GREF(baseType)** dst, const GREF(baseType)* src ) {	\
		if (0 == src) {													\
			::nwol::release(dst);										\
			return;														\
		}																\
		GREF(baseType)			* newRef	= 0;						\
		gcreate(&newRef);												\
		gcopy(newRef, src);												\
		GREF(baseType)			* oldRef	= *dst;						\
		(*dst)				= newRef;									\
		::nwol::release(&oldRef);										\
	}

#define __GDEFINE_COMMON_REF_FUNCTIONS(baseType)	\
	__GDEFINE_RELEASE(baseType);					\
	//__GDEFINE_ACQUIRE(baseType);	

#define __GDEFINE_COPY_REF_FUNCTIONS(baseType)	\
	__GDEFINE_CREATECUSTOM(baseType);			\
	__GDEFINE_CLONE(baseType);					\
	__GDEFINE_COPY(baseType);					\

//-------------------------------------------------------------// gcore_ref //------------------------------------------------------------------------// 
#if defined(DEBUG) || defined(_DEBUG)
#define __GCORE_REF_INIT_DEBUG_MEMBERS_NONS(baseType)		 uint64_t	GREF(baseType)::__breakAllocID = INVALID_ALLOC_ID;
#define __GCORE_REF_INIT_DEBUG_MEMBERS(NameSpace, baseType)	 uint64_t	NameSpace::GREF(baseType)::__breakAllocID = INVALID_ALLOC_ID;
#else
#define __GCORE_REF_INIT_DEBUG_MEMBERS_NONS(baseType)				
#define __GCORE_REF_INIT_DEBUG_MEMBERS(NameSpace, baseType)				
#endif

#	define __GCORE_REF_INIT_GCORE_REF_MEMBERS(NameSpace, baseType)		template<> const ::nwol::cue_t	NameSpace::GREF(baseType)::__kCue	= ::nwol::gsyslabel(#baseType).begin();
#	define __GCORE_REF_INIT_GCORE_REF_MEMBERS_NONS(baseType)			template<> const ::nwol::cue_t	GREF(baseType)::__kCue				= ::nwol::gsyslabel(#baseType).begin();

#define __GCORE_REF_INIT_STATIC_MEMBERS_NONS(baseType)	\
	__GCORE_REF_INIT_DEBUG_MEMBERS_NONS(baseType);		\
	__GCORE_REF_INIT_GCORE_REF_MEMBERS_NONS(baseType);												

#define __GCORE_REF_INIT_STATIC_MEMBERS(NameSpace, baseType)																								\
	__GCORE_REF_INIT_DEBUG_MEMBERS(NameSpace, baseType);																									\
	__GCORE_REF_INIT_GCORE_REF_MEMBERS(NameSpace, baseType);										

#if defined( _WIN64 )
#define REFCOUNT_T __int64
#elif defined (_WIN32)
#define REFCOUNT_T long
#else
#define REFCOUNT_T size_t
#endif
		
#endif // __GREF_DEFINITION_COMMON_H_293874922__
