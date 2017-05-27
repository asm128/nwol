#include "descriptor_manager.h"

::nwol::CDescriptorManager*							nwol::getDescriptorManager						()																														{	
	return &::nwol::CDescriptorManager::get();	
}

::nwol::error_t										nwol::CDescriptorManager::AddDescriptor			(const ::nwol::GDATA_TYPE* descriptor, uint32_t maxReadSize, ::nwol::array_view<const ::nwol::GDATA_TYPE>& arrayView)	{
	arrayView											= DescriptorData[0].get_array_view(0);	// the zero element is an empty string. Set in case of failure.
	if(0 == maxReadSize || 0 == descriptor[0]) // empty descriptor
		return 0;
	const uint8_t											key												= ((uint8_t)GTYPEID_TOTALBYTES(descriptor[0])) % MAX_DESCRIPTORDATA_ARRAYS;
	int32_t													itemIndex										= -1;
	nwol_pecall(DescriptorData[key].push_back(descriptor, maxReadSize, &itemIndex, arrayView), "Failed to add descriptor! descriptor: %s. key: 0x%X.", descriptor, (uint32_t)key);
	return 0;
}