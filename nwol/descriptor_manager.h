/// Copyright 2016-2017 - asm128
#include "unshrinkable_block_container.h"
#include "datatype.h"

#ifndef NWOL_DESCRIPTORMANAGER_H_61596898798741996481968498
#define NWOL_DESCRIPTORMANAGER_H_61596898798741996481968498

namespace nwol
{
#define MAX_DESCRIPTORDATA_ARRAYS 4
	class CDescriptorManager {
					::nwol::unordered_string_set<::nwol::GDATA_TYPE, 128>	DescriptorData	[MAX_DESCRIPTORDATA_ARRAYS];
	public:
					::nwol::error_t											AddDescriptor	(const ::nwol::GDATA_TYPE* types, uint32_t maxReadSize, nwol::array_view<const ::nwol::GDATA_TYPE>& arrayView);
		static		CDescriptorManager&										get				()	{
			static	CDescriptorManager												globalDescriptorManager;
			return globalDescriptorManager;
		}
	};

	CDescriptorManager* getDescriptorManager();
} // namespace

#endif // NWOL_DESCRIPTORMANAGER_H_61596898798741996481968498