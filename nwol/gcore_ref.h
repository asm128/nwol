#include "gcore_ref_globals.h"

#include "label.h"
#include "descriptor.h"
#include "multithread.h"

#ifndef __NWOL_REF_H__9823479023649023642890734__
#define __NWOL_REF_H__9823479023649023642890734__

namespace nwol
{
#pragma pack(push, 1)
	template <typename _tBase, ::nwol::GDATA_TYPE... _tArgs> 
	struct gcore_ref {																
		typedef						gcore_ref<_tBase, _tArgs...>			TRef;	
		typedef						_tBase									TBase;	
		__GPRECHECK();
									::nwol::refcount_t						ReferenceCount;	
									_tBase									* Instance;		
									const ::nwol::SReferenceGlobals			* Globals;		
		__GPOSTCHECK();
#if defined (NWOL_DEBUG_ENABLED)				
									uint64_t								AllocID; 
									uint32_t								Column;
		static						uint64_t								__breakAllocID;
#endif	
		inline						TRef*									acquire						()										{ 
			int64_t																	finalCount;
			if( 1 >= (finalCount = (int64_t)NWOL_INTERLOCKED_INCREMENT(ReferenceCount)) )	{															
				error_printf("Invalid reference count: %llu. Instance type: '%s'.", finalCount, get_type_name().begin());			
				PLATFORM_CRT_BREAKPOINT();											
			}			
			
			return this;	
		}
		inline						_tBase*									get							()							noexcept	{ return Instance;										}
		inline constexpr			const _tBase*							get							()					const	noexcept	{ return Instance;										}
		inline constexpr			const bool								shared						()					const	noexcept	{ return ReferenceCount > 1;							}

		// static members
		static						const ::nwol::cue_t						__kCue;	

		// static accessors
		static inline				const ::nwol::gdescriptor&				get_type_descriptor			()										{
			static constexpr	const ::nwol::GDATA_TYPE							dataTypes[]					= {_tArgs..., (GDATA_TYPE)0};									
			static				const ::nwol::gdescriptor							typeDescriptor				= ::nwol::gdescriptor(dataTypes, ~0U);
			static				const ::nwol::error_t								err							= ::nwol::validate_type_descriptor<_tBase>(typeDescriptor);	
			return typeDescriptor;																								
		}			

		static inline				const ::nwol::gsyslabel&				get_type_name				()										{
			static	const ::nwol::gsyslabel											typeName					= ::nwol::gsyslabel(__kCue, ~0U);
			return typeName;																									
		}
	}; // template struct
#pragma pack(pop)

	template <typename _tRef>
	static			bool													checkOverrun				(const _tRef* p)						{
		if(0 == p)
			return false;
		bool																		result						= false;
																											
		// reference check 
		if (BINFIBO != p->NWOL_DEBUG_CHECK_NAME_PRE				) { error_printf("Memory was overwritten before the reference metadata. AllocID: %llu. Value: %u. Expected: %u."	, p->AllocID, (uint32_t)p->NWOL_DEBUG_CHECK_NAME_PRE			, (uint32_t)BINFIBO); result = true; }																									
		if (BINFIBO != p->NWOL_DEBUG_CHECK_NAME_POST			) { error_printf("Memory was overwritten after the reference metadata. AllocID: %llu. Value: %u. Expected: %u."		, p->AllocID, (uint32_t)p->NWOL_DEBUG_CHECK_NAME_POST			, (uint32_t)BINFIBO); result = true; }																									
		// globals check 																																																														 
		if (BINFIBO != p->Globals->NWOL_DEBUG_CHECK_NAME_PRE	) { error_printf("Memory was overwritten before the globals section. AllocID: %llu. Value: %u. Expected: %u."		, p->AllocID, (uint32_t)p->Globals->NWOL_DEBUG_CHECK_NAME_PRE	, (uint32_t)BINFIBO); result = true; }																									
		if (BINFIBO != p->Globals->NWOL_DEBUG_CHECK_NAME_POST	) { error_printf("Memory was overwritten after the globals section. AllocID: %llu. Value: %u. Expected: %u."		, p->AllocID, (uint32_t)p->Globals->NWOL_DEBUG_CHECK_NAME_POST	, (uint32_t)BINFIBO); result = true; }																									
		if(result) {
			error_printf("AllocID: %u.", uint32_t(p->AllocID));
		}
		return result;
	}

	void			nwol_shutdown	();

} // namespace

#define NWOL_REF(baseType, ...)		::nwol::gcore_ref<baseType, __VA_ARGS__>				

#define GREF(Token)		gref_##Token
#define NWOH(Token)		grefh_##Token
#define GODS(Token)		NWOH(Token)

#define GDECLARE_REF(baseType, ...)		\
	typedef struct NWOL_REF(baseType, __VA_ARGS__)	GREF(baseType), *NWOH(baseType);							

#endif // __NWOL_REF_H__9823479023649023642890734__
