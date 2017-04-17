#ifndef __GREF_DEFINITION_COMMON_H_293874922__
#define __GREF_DEFINITION_COMMON_H_293874922__


#define __GDEFINE_COMMON_REF_FUNCTIONS(baseType)																																										\
	void grelease( GREF(baseType)** refToRelease ) {																																									\
		GREF(baseType)													* pRef		= *refToRelease;																													\
		*refToRelease												= 0;																																				\
		if(pRef)  																																																		\
			switch(pRef->Globals->ObjectCategory) {																																										\
			case ::nwol::GREF_CATEGORY_NCO: { ::nwol::gref_manager_nco<GREF(baseType)>* pManager = (::nwol::gref_manager_nco<GREF(baseType)>*)pRef->Globals->ReferenceManager; pManager->releaseRef(&pRef); break; }	\
			case ::nwol::GREF_CATEGORY_OBJ: { ::nwol::gref_manager_obj<GREF(baseType)>* pManager = (::nwol::gref_manager_obj<GREF(baseType)>*)pRef->Globals->ReferenceManager; pManager->releaseRef(&pRef); break; }	\
			case ::nwol::GREF_CATEGORY_POD: { ::nwol::gref_manager_pod<GREF(baseType)>* pManager = (::nwol::gref_manager_pod<GREF(baseType)>*)pRef->Globals->ReferenceManager; pManager->releaseRef(&pRef); break; }	\
			}																																																			\
		/*if( 0 == (*refToRelease) )						*/																																							\
		/*	return;											*/																																							\
		/*__g_##baseType##Manager.releaseRef(refToRelease);	*/																																							\
	}

#define __GDEFINE_COPY_REF_FUNCTIONS(baseType)	\
	void	gcreate	( GREF(baseType)** p1, const baseType& src )		{ __g_##baseType##Manager.createRef(p1, src); }

//-------------------------------------------------------------// gcore_ref //------------------------------------------------------------------------// 
#if defined(NWOL_DEBUG_ENABLED)
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

		
#endif // __GREF_DEFINITION_COMMON_H_293874922__
