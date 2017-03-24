#include "gref_manager_nco.h"

#ifndef __GREF_MANAGER_OBJ_H__2908374923__
#define __GREF_MANAGER_OBJ_H__2908374923__

//
#define __GDEFINE_CREATEOBJECT(baseType) \
	void gcreate	( GREF(baseType)** p1 )														{	__g_##baseType##Manager.createRef(p1);							}	\
	void gcreateAll	( GREF(baseType)** p1, uint32_t nCount  )									{	__g_##baseType##Manager.createRefs(p1, nCount);					}	\
	void gcreateAll	( GREF(baseType)** p1, const baseType* lstInstances, uint32_t nCount  )		{	__g_##baseType##Manager.createRefs(p1, lstInstances, nCount);	}

#define __GDEFINE_ALLOC(baseType )																														\
	void galloc		( GREF(baseType)** p1 )														{	__g_##baseType##Manager.allocRef(p1); }				\
	void gallocAll	( GREF(baseType)** p1, uint32_t nCount )									{	__g_##baseType##Manager.allocRefs(p1, nCount); }

namespace nwol
{
	template <typename _TRef> class gref_manager_obj : public gref_manager_nco<_TRef, GREF_MANAGER_TYPE_OBJ>
	{
		typedef				gref_manager_nco<_TRef, GREF_MANAGER_TYPE_OBJ>	base_manager_type	;
		using				base_manager_type::								createRef_noinit	;
		typedef				typename _TRef::TBase							_tBase;
	public:
		static inline		gref_manager_obj<_TRef>&						get					()																				{
			static gref_manager_obj<_TRef>	managerInstance;
			return managerInstance;
		}
		using				base_manager_type::								allocRef;
		using				base_manager_type::								allocRefs;

		inline constexpr													gref_manager_obj	(void(*_funcFreeR)(_TRef**))													: base_manager_type::gref_manager_nco(_funcFreeR)	{}
		inline				void											copyInstance		(_TRef* dst, const _tBase* src)								const				{ *dst->Instance = *src;	PLATFORM_CRT_CHECK_MEMORY(); }
							void											cloneInstance		(_TRef** dst, const _TRef* src)								const				{
			if (0 == src) {
				::nwol::release(dst);
				return;
			}
			gcreate(dst, *src->get());
		} // func
							void											createRef			(_TRef** p1)																	{
			_TRef																	* newRef			= createRef_noinit();
			if (newRef)
				new (newRef->Instance) _tBase();
			_TRef																	* oldRef			= (*p1);
			(*p1)																= newRef;
			::nwol::release(&oldRef);
		}
							void											createRef			(_TRef** p1, const _tBase& _InitObject)											{
			_TRef																	* newRef			= createRef_noinit();
			if (newRef)
				new (newRef->Instance) _tBase(_InitObject);
			_TRef																	* oldRef			= (*p1);
			(*p1)																= newRef;
			::nwol::release(&oldRef);
		}
							void											createRefs			(_TRef** p1, uint32_t nCount)													{
			allocRefs(p1, nCount);
			_TRef																	* newRef;
			for( uint32_t i=0; i<nCount; i++ )
				if (0 != (newRef = p1[i]))
					new (newRef->Instance) _tBase();
		}
							void											createRefs			(_TRef** p1, const _tBase* lstInstances, uint32_t nCount)						{
			allocRefs(p1, nCount);
			_TRef																	* newRef;
			for( uint32_t i=0; i<nCount; i++ )
				if( 0 != (newRef = p1[i]))
					new (newRef->Instance) _tBase(lstInstances[i]);
		}

	}; // __CObjDestructor


}// namespace

#endif // __GREF_MANAGER_OBJ_H__2908374923__

