/// Copyright 2016-2017 - asm128
#include "nwol_datatype.h"
#include "nwol_array_view.h"

//#include <cstdio>

#ifndef NWOL_DESCRIPTOR_H_61596841996481968498
#define NWOL_DESCRIPTOR_H_61596841996481968498

namespace nwol
{
	class CDescriptorManager;
#pragma pack(push, 1)
	class gdescriptor : public ::nwol::array_view<const ::nwol::GDATA_TYPE> {
							CDescriptorManager				* DescriptorManager						= nullptr;
	public:
		inline constexpr									gdescriptor								()																			= default;
		inline constexpr									gdescriptor								(const gdescriptor& other)													= default;
															gdescriptor								(const ::nwol::GDATA_TYPE* descriptor, uint32_t size);			// <------ this is the only constructor that actually matters.
		template<size_t Size> 
		inline constexpr									gdescriptor								(const ::nwol::GDATA_TYPE (&descriptor)[Size])								: gdescriptor(descriptor, (uint32_t)Size)															{}

		inline				gdescriptor&					operator	=							(const gdescriptor&  other)										noexcept	= default;
							bool							operator	==							(const gdescriptor& other)								const	noexcept	;
		inline				bool							operator	!=							(const gdescriptor& other)								const	noexcept	{ return !operator==(other); }

							uint32_t						save									(char* out_pMemoryBuffer)								const				;
							::nwol::error_t					load									(const char* in_pMemoryBuffer)												;
	};
#pragma pack(pop)

	template<typename _tBase>
	static inline		const ::nwol::error_t			validate_type_descriptor				(const ::nwol::gdescriptor& descriptor)		{						
		uint32_t												totalBytes								= 0;
		for(uint32_t i = 0, count = descriptor.size(); i < count; ++i)
			totalBytes											+= GTYPEID_TOTALBYTES(descriptor[i]);
		ree_if(totalBytes != sizeof(_tBase), "Descriptor and type size don't match. Descriptor size: %u. sizeof(): %u", totalBytes, (uint32_t)sizeof(_tBase));				
		return 0;
	}
};

#endif // NWOL_DESCRIPTOR_H_61596841996481968498