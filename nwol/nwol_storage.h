/// Copyright 2017 - asm128
#include "nwol_label.h"
#include "nwol_array.h"

#ifndef NWOL_STORAGE_H_98234687
#define NWOL_STORAGE_H_98234687

namespace nwol
{
	struct SPathContents {
		::nwol::array_obj<::nwol::glabel>		Files	;
		::nwol::array_obj<::nwol::glabel>		Folders	;
	};

	int32_t									listFiles				(const char* directory, ::nwol::array_obj<::nwol::glabel>& fileNames);
} // namespace

#endif // NWOL_STORAGE_H_98234687
