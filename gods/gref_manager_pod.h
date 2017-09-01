#include "gref_manager_nco.h"

#include <stdio.h>
#include <vector>

#ifndef NWOL_GREF_MANAGER_POD_H_287343426872
#define NWOL_GREF_MANAGER_POD_H_287343426872

#define __GDEFINE_CREATEDATA( baseType )																											\
	void gcreate( GREF(baseType)** p2 )														{ __g_##baseType##Manager.allocRef(p2);				}	\
	void gcreateAll( GREF(baseType)** p2, uint32_t nCount )									{ __g_##baseType##Manager.allocRefs(p2, nCount);	}	\
	void gcreateAll( GREF(baseType)** p2, const baseType* lstInstances, uint32_t nCount )	{ __g_##baseType##Manager.createRefs(p2, lstInstances, nCount); }

namespace nwol
{
	template <typename _tRef> class gref_manager_pod : public gref_manager_nco<_tRef, GREF_CATEGORY_POD>
	{
		typedef			gref_manager_nco<_tRef, GREF_CATEGORY_POD>	base_manager_type;
		typedef			typename _tRef::TBase						_tBase;
		using			base_manager_type::							Globals;
		using			base_manager_type::							createRef_noinit;

	public:
		static inline	gref_manager_pod<_tRef>&					get						()																		{
			static gref_manager_pod<_tRef>									managerInstance;
			return managerInstance;
		}
		using			base_manager_type::							allocRef;
		using			base_manager_type::							allocRefs;

																	gref_manager_pod		(void(*_funcFreeR)(_tRef**))											: base_manager_type::gref_manager_nco(_funcFreeR)			{}

						void										createRef				(_tRef** p1, const _tBase& _InitData)									{
			_tRef															* newRef				= createRef_noinit();
			if( newRef )
				::nwol::podcpy(newRef->Instance, &_InitData);

			_tRef															* oldRef				= (*p1);
			(*p1)														= newRef;
			::nwol::release(&oldRef);
		}
						void										createRefs				(_tRef** p1, const _tBase* lstInstances, uint32_t nCount)				{
			allocRefs(p1, nCount);
			_tRef															* newRef;
			for( uint32_t i=0; i<nCount; i++ ) {
				if (0 != (newRef = p1[i]))
					::nwol::podcpy(newRef->Instance, &lstInstances[i]);
			}
		}
	}; // manager

} // namespace 

#endif // NWOL_GREF_MANAGER_POD_H_287343426872
