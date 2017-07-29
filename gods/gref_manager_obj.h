#include "gref_manager_nco.h"

#ifndef NWOL_GREF_MANAGER_OBJ_H_2908374923
#define NWOL_GREF_MANAGER_OBJ_H_2908374923

//
#define __GDEFINE_CREATEOBJECT(baseType) \
	void gcreate	( GREF(baseType)** p1 )														{	__g_##baseType##Manager.createRef(p1);							}	\
	void gcreateAll	( GREF(baseType)** p1, uint32_t nCount  )									{	__g_##baseType##Manager.createRefs(p1, nCount);					}	\
	void gcreateAll	( GREF(baseType)** p1, const baseType* lstInstances, uint32_t nCount  )		{	__g_##baseType##Manager.createRefs(p1, lstInstances, nCount);	}

#define __GDEFINE_ALLOC(baseType )																														\
	void galloc		( GREF(baseType)** p1 )														{	__g_##baseType##Manager.allocRef(p1);			}	\
	void gallocAll	( GREF(baseType)** p1, uint32_t nCount )									{	__g_##baseType##Manager.allocRefs(p1, nCount);	}

namespace nwol
{
	template <typename _tRef> class gref_manager_obj : public gref_manager_nco<_tRef, GREF_CATEGORY_OBJ> {
		typedef				gref_manager_nco<_tRef, GREF_CATEGORY_OBJ>		base_manager_type	;
		using				base_manager_type::								createRef_noinit	;
		typedef				typename _tRef::TBase							_tBase;
	public:
		static inline		gref_manager_obj<_tRef>&						get					()																				{
			static gref_manager_obj<_tRef>	managerInstance;
			return managerInstance;
		}
		using				base_manager_type::								allocRef;
		using				base_manager_type::								allocRefs;

		inline constexpr													gref_manager_obj	(void(*_funcFreeR)(_tRef**))													: base_manager_type::gref_manager_nco(_funcFreeR)	{}
							void											createRef			(_tRef** p1)																	{
			_tRef																	* newRef			= createRef_noinit();
			if (newRef)
				new (newRef->Instance) _tBase();
			_tRef																	* oldRef			= (*p1);
			(*p1)																= newRef;
			::nwol::release(&oldRef);
		}
							void											createRef			(_tRef** p1, const _tBase& _InitObject)											{
			_tRef																	* newRef			= createRef_noinit();
			if (newRef)
				new (newRef->Instance) _tBase(_InitObject);
			_tRef																	* oldRef			= (*p1);
			(*p1)																= newRef;
			::nwol::release(&oldRef);
		}
							void											createRefs			(_tRef** p1, uint32_t nCount)													{
			allocRefs(p1, nCount);
			_tRef																	* newRef;
			for( uint32_t i=0; i<nCount; i++ )
				if (0 != (newRef = p1[i]))
					new (newRef->Instance) _tBase();
		}
							void											createRefs			(_tRef** p1, const _tBase* lstInstances, uint32_t nCount)						{
			allocRefs(p1, nCount);
			_tRef																	* newRef;
			for( uint32_t i=0; i<nCount; i++ )
				if( 0 != (newRef = p1[i]))
					new (newRef->Instance) _tBase(lstInstances[i]);
		}
	}; // __CObjDestructor


}// namespace

#endif // NWOL_GREF_MANAGER_OBJ_H_2908374923
