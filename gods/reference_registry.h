#include "gbuffer.h"

#ifndef __GINSTANCECONTAINER_H__684164684894899948498897653242__
#define __GINSTANCECONTAINER_H__684164684894899948498897653242__

namespace nwol
{
	class CReferenceContainer {
				::nwol::gapointer<void>			Data					;
				::nwol::galabel					DataNames				;
				::nwol::galabel					DataDescriptions		;

		inline	 void							release_all				()																	{
			for(uint32_t i=0, count=Data.size(); i<count; i++) {
				::nwol::GODS(SBuffer)						* rangeRef				= (::nwol::GODS(SBuffer)*)&Data.begin()[i];
				::nwol::release(rangeRef);
			}
		}
		inline	::nwol::error_t					validateIndex			(::nwol::id_t index)										const	{ return (index < 0 || ((uint32_t)index) >= DataNames.size()) ? -1 : 0; }
	public:
												~CReferenceContainer	()																	{ release_all(); }
		inline	::nwol::error_t					GetDescription			(::nwol::id_t index, ::nwol::glabel& ref)					const	{ reterr_error_if(validateIndex(index), "Invalid instance index: '%i'", index); ref = DataDescriptions[index];	return 0; }
		inline	::nwol::error_t					GetTypeName				(::nwol::id_t index, ::nwol::glabel& ref)					const	{ reterr_error_if(validateIndex(index), "Invalid instance index: '%i'", index); ref = DataNames[index];			return 0; }

		template <typename _tRef> 
		inline	::nwol::error_t					SetReference			(::nwol::id_t index, ::nwol::gptr_nco<_tRef>& ref)					{
			reterr_error_if(validateIndex(index), "Invalid instance index: '%i'", index);
			if(ref.get_type_name() != DataNames[index])
				warning_printf("Changing reference type at index %u. Previous type: %s. New type: %s", DataNames[index].begin(), ref.get_type_name().begin());

			DataNames.begin()[index]				= ref.get_type_name();
			*reinterpret_cast<_tRef* const *>(&Data.begin()[index]) = ref.acquire();
			return 0;
		}

		template <typename _tRef> 
		inline	::nwol::error_t					AcquireReference		(::nwol::id_t index, ::nwol::gptr_nco<_tRef>& ref)			const	{
			reterr_error_if(validateIndex(index), "Invalid instance index: '%i'", index);
			if(ref.get_type_name() != DataNames[index])
				warning_printf("Casting stored reference to a different type! Stored type: '%s'. Casted type: '%s'.", DataNames[index].begin(), ref.get_type_name().begin());

			ref										= ::nwol::acquire(*reinterpret_cast<_tRef* const *>(&Data.begin()[index]));
			return 0;
		}

		template <typename _tRef> 
		inline	::nwol::error_t					PushReference			(const ::nwol::gptr_nco<_tRef>& ref, ::nwol::id_t* index, const ::nwol::glabel description = ::nwol::glabel::statics().empty)	{
			uint32_t									newIndex				= DataNames.size();

			Data				.push(ref.acquire());
			DataNames			.push(ref.get_type_name());
			DataDescriptions	.push(description);

			if(index)
				*index									= newIndex;

			return 0;
		}

				uint32_t						Save					(char* out_pMemoryBuffer)									const;
				uint32_t						Load					(const char* in_pMemoryBuffer);
	};	// class
	GDECLARE_OBJ(CReferenceContainer)
}	// namespace

#endif // __GINSTANCECONTAINER_H__684164684894899948498897653242__