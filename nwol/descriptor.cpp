#include "descriptor.h"

#include "descriptor_manager.h"
#include "stype.h"

							nwol::gdescriptor::gdescriptor			(const ::nwol::GDATA_TYPE* descriptor, uint32_t size)	: DescriptorManager(getDescriptorManager())	{
	throw_if(errored(DescriptorManager->AddDescriptor(descriptor, size, *this)), "unknown", "Failed to store descriptor! Descriptor size: %u", size);
}

bool						nwol::gdescriptor::operator	==			(const gdescriptor& other)								const	noexcept							{ 
		 if(Data				== other.Data				)	return true;
	else if(DescriptorManager	== other.DescriptorManager	)	return false;
	else if(Count				!= other.Count				)	return false;
	else														return 0 == memcmp(Data, other.Data, Count*sizeof(::nwol::GDATA_TYPE));
}

uint32_t					nwol::gdescriptor::save					(char* out_pMemoryBuffer)								const										{
	uint32_t						totalBytes								= 0;
	sint32							descriptorSize							= (int32_t)size();
	totalBytes					+= descriptorSize.write(out_pMemoryBuffer);
	if(descriptorSize) {
		if(out_pMemoryBuffer)
			memcpy(&out_pMemoryBuffer[totalBytes], begin(), descriptorSize*sizeof(::nwol::GDATA_TYPE)); 
	}
	return totalBytes			+= descriptorSize;
}

uint32_t					nwol::gdescriptor::load					(const char* in_pMemoryBuffer)																		{
	uint32_t						totalBytes								= 0;
	sint32							descriptorSize							= 0;
	totalBytes					+= descriptorSize.read(in_pMemoryBuffer);
	if(descriptorSize) {
		::nwol::GDATA_TYPE				* a										= (::nwol::GDATA_TYPE*)::nwol::nwol_malloc(descriptorSize*sizeof(::nwol::GDATA_TYPE));
		throw_if(0 == a, "out_of_memory", "Failed to allocate memory for descriptor of size %u.", (uint32_t)descriptorSize)
		else {
			if(in_pMemoryBuffer) {
				memcpy(a, &in_pMemoryBuffer[totalBytes], descriptorSize*sizeof(::nwol::GDATA_TYPE)); 
				*this						= {a, descriptorSize};
			}
			else {
				*this						= {};
			}

			::nwol::nwol_free(a);
		}
	}
	return totalBytes			+= descriptorSize;
}
