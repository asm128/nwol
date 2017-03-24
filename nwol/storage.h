#include "label.h"
#include "nwol_array.h"

#ifndef __STORAGE_H__98234687__
#define __STORAGE_H__98234687__

namespace nwol
{
	struct SPathContents {
		::nwol::array_obj<::nwol::glabel>		Files	;
		::nwol::array_obj<::nwol::glabel>		Folders	;
	};

	int32_t									listFiles		(const char* directory, ::nwol::array_obj<::nwol::glabel>& fileNames);

} // namespace

#endif // __STORAGE_H__98234687__