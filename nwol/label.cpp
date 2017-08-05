/// Copyright 2016-2017 - asm128
//#pragma warning(disable:4005)
#include "label.h"

#include "label_manager.h"
#include "stype.h"
#include "platform_handle_wrapper.h"

#include <string>

//------------------------------------------------------------------- gsyslabel ---------------------------------------------------------------------------------------------------------
									nwol::gsyslabel::gsyslabel		(const char_t* label, uint32_t size)																{
	LabelManager							= getSystemLabelManager();
	error_if(errored(LabelManager->AddLabel(label, size, *this)), "Failed to store label!");
}

//------------------------------------------------------------------- glabel ---------------------------------------------------------------------------------------------------------
									nwol::glabel::glabel			(const char_t* label, uint32_t size)							: LabelManager(getLabelManager())	{
	error_if(errored(LabelManager->AddLabel(label, size, *this)), "Failed to store label!");
}

bool								nwol::glabel::operator	==		(const nwol::glabel& other)				const	noexcept										{ 
		 if(Count			!= other.Count			)	return false;
	else if(0 == Count && Count == other.Count		)	return true;	// Empty labels are always equal regardless the Data pointer
	else if(Data			== other.Data			)	return true;
	else if(LabelManager	== other.LabelManager	)	return false;
	else												return 0 == memcmp(Data, other.Data, Count);
}

uint32_t							nwol::glabel::save				(char_t* out_pMemoryBuffer)				const													{
	static constexpr	const uint32_t		headerBytes						= (uint32_t)sizeof(uint32_t);
						const uint32_t		arrayBytes						= (uint32_t)(Count * sizeof(char_t));
	if(out_pMemoryBuffer) {
		*(uint32_t*)out_pMemoryBuffer		= Count;
		if(arrayBytes) 
			memcpy(&out_pMemoryBuffer[headerBytes], Data, arrayBytes); 
	}
	return headerBytes + arrayBytes;
}

::nwol::error_t						nwol::glabel::load				(const char_t* in_pMemoryBuffer)																	{
	reterr_error_if(0 == in_pMemoryBuffer, "Cannot load label from a null pointer!");
	const uint32_t							headerBytes						= (uint32_t)sizeof(uint32_t);
	const uint32_t							labelSize						= *(const uint32_t*)in_pMemoryBuffer;
	*this								= labelSize ? ::nwol::glabel((const char_t*)&in_pMemoryBuffer[headerBytes], labelSize) : ::nwol::glabel::statics().empty;
	return headerBytes + labelSize;
}

::nwol::error_t						nwol::glabel::save				(FILE* out_pMemoryBuffer)				const													{
	const sint32							labelSize						= (int32_t)size();
	nwol_necall(labelSize.write(out_pMemoryBuffer), "Failed to write label to file! Label: '%s'.", begin());
	if(labelSize) {
		reterr_error_if(labelSize != (int32_t)fwrite(begin(), sizeof(char_t), labelSize, out_pMemoryBuffer), "Failed to write label to file! Label: '%s'.", begin());
	}
	return 0;
}

::nwol::error_t						nwol::glabel::load				(FILE* in_pMemoryBuffer)																		{
	sint32									labelSize						= {};
	nwol_necall(labelSize.read(in_pMemoryBuffer), "%s", "Failed to read label from file!");
	if(labelSize) {
		::nwol::auto_nwol_free					a;
		a.Handle							= (char_t*)::nwol::nwol_malloc(labelSize);
		reterr_error_if(0 == a, "Failed to allocate memory for label of size %u.", (uint32_t)labelSize);
		if(labelSize != (int32_t)fread(a, sizeof(char_t), labelSize, in_pMemoryBuffer), "%s", "Failed to read label from file!") {
			error_printf("Failed to read from file label of size: %u bytes.", labelSize);
			*this								= glabel::statics().empty;
			return -1;
		}
		else
			*this								= glabel((const char_t*)a.Handle, labelSize);
	}
	return 0;
}