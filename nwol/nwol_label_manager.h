/// Copyright 2016-2017 - asm128
#include "nwol_unshrinkable_block_container.h"

#ifndef NWOL_GLABELMANAGER_H_61596898798741996481968498
#define NWOL_GLABELMANAGER_H_61596898798741996481968498

namespace nwol
{
//#define MAX_LABELDATA_ARRAYS 16
	class CLabelManager {
		static constexpr	const uint32_t												MAX_LABELDATA_ARRAYS								= 16;
							::nwol::unordered_string_set<char, 4096>					LabelData		[MAX_LABELDATA_ARRAYS]				= {};

		inline				CLabelManager&												operator=											(const CLabelManager& other)															= delete;

	public:	//------------------------------------------------------				--------			-----
							::nwol::error_t												AddLabel											(const char* text, uint32_t maxReadSize, nwol::array_view<const char>& arrayView);
		static				CLabelManager&												get													()																						{
			static	CLabelManager																globalLabelManager;
			return globalLabelManager;
		}
	};

						CLabelManager*												getLabelManager										();
						CLabelManager*												getSystemLabelManager								();
} // namespace

#endif // NWOL_GLABELMANAGER_H_61596898798741996481968498
