#include "datatype.h"
#include "array_view.h"
#include "nwol_error.h"

//#include <cstdio>

#ifndef __DESCRIPTOR_H_61596841996481968498__
#define __DESCRIPTOR_H_61596841996481968498__

namespace nwol
{
	class CDescriptorManager;
#pragma pack(push, 1)
	class gdescriptor : public ::nwol::array_view<const ::nwol::GDATA_TYPE> {
							CDescriptorManager		* DescriptorManager			= nullptr;
	public:
		inline constexpr							gdescriptor					()																			= default;
		inline constexpr							gdescriptor					(const gdescriptor& other)													= default;
													gdescriptor					(const ::nwol::GDATA_TYPE* descriptor, uint32_t size);			// <------ this is the only constructor that actually matters.
		template<size_t Size> 
		inline constexpr							gdescriptor					(const ::nwol::GDATA_TYPE (&descriptor)[Size])								: gdescriptor(descriptor, (uint32_t)Size)															{}

		inline				gdescriptor&			operator	=				(const gdescriptor&  other)										noexcept	= default;
							bool					operator	==				(const gdescriptor& other)								const	noexcept	;
		inline				bool					operator	!=				(const gdescriptor& other)								const	noexcept	{ return !operator==(other); }

							uint32_t				save						(char* out_pMemoryBuffer)								const				;
							uint32_t				load						(const char* in_pMemoryBuffer)												;
	};
#pragma pack(pop)

	template<typename _tBase>
	static inline	const ::nwol::error_t		validate_type_descriptor	(const ::nwol::gdescriptor& descriptor)		{						
		uint32_t										totalBytes					= 0;
		for(uint32_t i=0, count=descriptor.size(); i<count; i++)
			totalBytes									+= GTYPEID_TOTALBYTES(descriptor[i]);
		reterr_error_if(totalBytes != sizeof(_tBase), "Descriptor and type size don't match. Descriptor size: %u. sizeof(): %u", totalBytes, (uint32_t)sizeof(_tBase));				
		return 0;
	}
};

#endif // __DESCRIPTOR_H_61596841996481968498__