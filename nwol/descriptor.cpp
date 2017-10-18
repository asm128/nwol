/// Copyright 2016-2017 - asm128
#include "nwol_descriptor.h"

#include "nwol_descriptor_manager.h"
#include "stype.h"

												nwol::gdescriptor::gdescriptor			(const ::nwol::GDATA_TYPE* descriptor, uint32_t size)							: DescriptorManager(getDescriptorManager())	{ throw_if(errored(DescriptorManager->AddDescriptor(descriptor, size, *this)), ::std::exception("Out of range - Out of memory."), "Failed to store descriptor! Descriptor size: %u", size); }
bool											nwol::gdescriptor::operator	==			(const gdescriptor& other)									const	noexcept	{ 
		 if(0 == Count && Count == other.Count				)	return true;	// Empty labels are always equal regardless the Data pointer
	else if(Data				== other.Data				)	return true;
	else if(DescriptorManager	== other.DescriptorManager	)	return false;
	else if(Count				!= other.Count				)	return false;
	else														return 0 == memcmp(Data, other.Data, Count*sizeof(::nwol::GDATA_TYPE));
}

uint32_t										nwol::gdescriptor::save					(char* out_pMemoryBuffer)									const				{
	static constexpr	const uint32_t					headerBytes								= (uint32_t)sizeof(uint32_t);
						const uint32_t					arrayBytes								= (uint32_t)(Count * sizeof(::nwol::GDATA_TYPE));
	if(out_pMemoryBuffer) {
		*(uint32_t*)out_pMemoryBuffer					= Count;
		if(arrayBytes) 
			memcpy(&out_pMemoryBuffer[headerBytes], Data, arrayBytes); 
	}
	return headerBytes + arrayBytes;
}

::nwol::error_t									nwol::gdescriptor::load					(const char* in_pMemoryBuffer)													{
	ree_if(0 == in_pMemoryBuffer, "Cannot load label from a null pointer!");
	static constexpr	const uint32_t					headerBytes								= (uint32_t)sizeof(uint32_t);
						const uint32_t					descriptorSize							= *(const uint32_t*)in_pMemoryBuffer;
	*this											= descriptorSize ? ::nwol::gdescriptor((const ::nwol::GDATA_TYPE*)&in_pMemoryBuffer[headerBytes], descriptorSize) : ::nwol::gdescriptor();
	return headerBytes + descriptorSize;
}
