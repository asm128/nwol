#include "label_manager.h"

::nwol::CLabelManager*		nwol::getLabelManager			()	{ return &nwol::CLabelManager::get(); }
::nwol::CLabelManager*		nwol::getSystemLabelManager		()	{
	static ::nwol::CLabelManager		sysLabelManager;
	return &sysLabelManager;
}

::nwol::error_t				nwol::CLabelManager::AddLabel	(const char* text, uint32_t maxReadSize, nwol::array_view<const char>& arrayView) {
	arrayView										= LabelData[0].get_array_view(0);	// the zero element is an empty string. Set in case of failure.

	if(0 == maxReadSize) // empty string
		return 0;

	if(0 == text || 0 == text[0]) // empty string
		return 0;

	unsigned char										key			= ((unsigned char)text[0]) % MAX_LABELDATA_ARRAYS;
	int32_t												itemIndex	= -1;
	reterr_error_if(0 > LabelData[key].push_back(text, maxReadSize, &itemIndex, arrayView), "Failed to add label! label: %s. key: 0x%X.", text, (uint32_t)key);

	return 0;
}
