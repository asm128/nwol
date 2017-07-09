/// Copyright 2016-2017 - asm128
#include "unshrinkable_block_container.h"

#ifndef __GLABELMANAGER_H_61596898798741996481968498__
#define __GLABELMANAGER_H_61596898798741996481968498__

namespace nwol
{
#define MAX_LABELDATA_ARRAYS 128
	class CLabelManager {
					::nwol::unordered_string_set<char, 1024>	LabelData				[MAX_LABELDATA_ARRAYS];
	public:	//--------------------------------------------------------------			-----
					::nwol::error_t								AddLabel				(const char* text, uint32_t maxReadSize, nwol::array_view<const char>& arrayView);
		static		CLabelManager&								get						()																					{
			static	CLabelManager										globalLabelManager;
			return	globalLabelManager;
		}
	};

				CLabelManager*								getLabelManager			();
				CLabelManager*								getSystemLabelManager	();
} // namespace

#endif // __GLABELMANAGER_H_61596898798741996481968498__