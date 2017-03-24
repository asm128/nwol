#include "gref_manager_nco.h"

#include <stdio.h>
#include <vector>

#ifndef __GREF_MANAGER_POD_H__287343426872__
#define __GREF_MANAGER_POD_H__287343426872__

#define __GDEFINE_CREATEDATA( baseType )																											\
	void gcreate( GREF(baseType)** p2 )														{ __g_##baseType##Manager.allocRef(p2);				}	\
	void gcreateAll( GREF(baseType)** p2, uint32_t nCount )									{ __g_##baseType##Manager.allocRefs(p2, nCount);	}	\
	void gcreateAll( GREF(baseType)** p2, const baseType* lstInstances, uint32_t nCount )	{ __g_##baseType##Manager.createRefs(p2, lstInstances, nCount); }

namespace nwol
{
	template <typename _TRef> class gref_manager_pod : public gref_manager_nco<_TRef, GREF_MANAGER_TYPE_POD>
	{
		typedef		gref_manager_nco<_TRef, GREF_MANAGER_TYPE_POD>	base_manager_type;
		typedef			typename _TRef::TBase						_tBase;
		using			base_manager_type::							Globals;
		using			base_manager_type::							createRef_noinit;

	public:
		static inline	gref_manager_pod<_TRef>&					get						()																		{
			static gref_manager_pod<_TRef>	managerInstance;
			return managerInstance;
		}
		using			base_manager_type::							allocRef;
		using			base_manager_type::							allocRefs;

																	gref_manager_pod		(void(*_funcFreeR)(_TRef**))											: base_manager_type::gref_manager_nco(_funcFreeR)			{}

						void										createRef				(_TRef** p1, const _tBase& _InitData)									{
			_TRef															* newRef				= createRef_noinit();
			if( newRef )
				::nwol::podcpy(newRef->Instance, &_InitData);

			_TRef															* oldRef				= (*p1);
			(*p1)														= newRef;
			::nwol::release(&oldRef);
		}

						void										createRefs				(_TRef** p1, const _tBase* lstInstances, uint32_t nCount)				{
			allocRefs(p1, nCount);
			_TRef															* newRef;
			for( uint32_t i=0; i<nCount; i++ ) {
				if (0 != (newRef = p1[i]))
					::nwol::podcpy(newRef->Instance, &lstInstances[i]);
			}
		}

						void										copyInstance			(_TRef* dst, const _tBase* src)									const	{
			if (0 == dst && 0 == src)
				return;
			else if (0 == dst) {
				error_printf("Cannot copy %s contents into a null pointer!", _TRef::get_type_name().begin());
				return;
			}
			else if (0 == src)
				memset(dst->get(), 0, dst->Globals->DataSize);
			else
				::nwol::podcpy(dst->Instance, src);
#if defined(DEBUG) || defined(_DEBUG)
			::nwol::checkOverrun(dst);
#endif
			PLATFORM_CRT_CHECK_MEMORY();
		} // func
	}; // manager

} // namespace 

#endif // __GREF_MANAGER_POD_H__287343426872__
