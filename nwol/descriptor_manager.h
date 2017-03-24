#include "unshrinkable_block_container.h"
#include "datatype.h"

#ifndef __DESCRIPTORMANAGER_H_61596898798741996481968498__
#define __DESCRIPTORMANAGER_H_61596898798741996481968498__

namespace nwol
{
#define MAX_DESCRIPTORDATA_ARRAYS 4
	class CDescriptorManager
	{
					::nwol::unordered_string_set<::nwol::GDATA_TYPE, 128>	DescriptorData	[MAX_DESCRIPTORDATA_ARRAYS];
	public:
					::nwol::error_t											AddDescriptor	(const ::nwol::GDATA_TYPE* types, uint32_t maxReadSize, nwol::array_view<const ::nwol::GDATA_TYPE>& arrayView);
		static		CDescriptorManager&										get				()	{
			static CDescriptorManager												globalDescriptorManager;
			return globalDescriptorManager;
		}
	};

	CDescriptorManager* getDescriptorManager();
} // namespace

#endif // __DESCRIPTORMANAGER_H_61596898798741996481968498__