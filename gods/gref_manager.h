#include "gref_manager_obj.h"
#include "gref_manager_pod.h"

#ifndef GREF_MANAGER_H_9238740928734
#define GREF_MANAGER_H_9238740928734
	template<typename _tRef>	void							_nwol_internal_release			(_tRef** refToRelease)								{
//		_tRef															* pRef							= *refToRelease;
//		if(pRef) {
//			*refToRelease												= 0;
			switch(pRef->Globals->ObjectCategory) {
			case GREF_CATEGORY_NCO: { ::nwol::gref_manager_nco<_tRef>* pManager = (::nwol::gref_manager_nco<_tRef>*)pRef->Globals->ReferenceManager; pManager->releaseRef(&pRef); break; }
			case GREF_CATEGORY_OBJ: { ::nwol::gref_manager_obj<_tRef>* pManager = (::nwol::gref_manager_obj<_tRef>*)pRef->Globals->ReferenceManager; pManager->releaseRef(&pRef); break; }
			case GREF_CATEGORY_POD: { ::nwol::gref_manager_pod<_tRef>* pManager = (::nwol::gref_manager_pod<_tRef>*)pRef->Globals->ReferenceManager; pManager->releaseRef(&pRef); break; }
			}
		//}
	}
#endif // GREF_MANAGER_H_9238740928734