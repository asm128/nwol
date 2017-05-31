/// Copyright 2016-2017 - asm128
//#pragma warning(disable:4005)
#include "label.h"

#include "label_manager.h"
#include "stype.h"
#include "platform_handle_wrapper.h"

#include <string>

//
//static const nwol::glabel_statics	init_statics = {
//	""		// empty;
//	"/"		// slash;
//	"\\"	// backslash;
//	"?"		// question_mark;
//	"!"		// exclamation_mark;
//	"."		// dot;
//	","		// comma;
//};
//const nwol::glabel_statics&	glabel::statics = init_statics;

//------------------------------------------------------------------- gsyslabel ---------------------------------------------------------------------------------------------------------
									nwol::gsyslabel::gsyslabel		(const char* label, uint32_t size)																{
	LabelManager							= getSystemLabelManager();
	error_if(errored(LabelManager->AddLabel(label, size, *this)), "Failed to store label!");
}

//------------------------------------------------------------------- glabel ---------------------------------------------------------------------------------------------------------
									nwol::glabel::glabel			(const char* label, uint32_t size)							: LabelManager(getLabelManager())	{
	error_if(errored(LabelManager->AddLabel(label, size, *this)), "Failed to store label!");
}

bool								nwol::glabel::operator	==		(const nwol::glabel& other)				const	noexcept										{ 
		 if(0 == Count && Count == other.Count		)	return true;	// Empty labels are always equal regardless the Data pointer
	else if(Data			== other.Data			)	return true;
	else if(LabelManager	== other.LabelManager	)	return false;
	else if(Count			!= other.Count			)	return false;
	else												return 0 == memcmp(Data, other.Data, Count);

}

uint32_t							nwol::glabel::save				(char* out_pMemoryBuffer)				const													{
	uint32_t								totalBytes						= 0;
	::nwol::sint32							labelSize						= (int32_t)size();
	totalBytes							+= labelSize.write(out_pMemoryBuffer);
	if(labelSize) {
		if(out_pMemoryBuffer)
			memcpy(&out_pMemoryBuffer[totalBytes], begin(), labelSize*sizeof(char)); 
	}
	return totalBytes					+= labelSize;
}

uint32_t							nwol::glabel::load				(const char* in_pMemoryBuffer)																	{
	uint32_t								totalBytes						= 0;
	sint32									labelSize						= 0;
	totalBytes							+= labelSize.read(in_pMemoryBuffer);
	if(labelSize) {
		::nwol::auto_nwol_free					a;
		a.Handle							= (char*)::nwol::nwol_malloc(labelSize);
		throw_if(0 == a, "out_of_memory", "Failed to allocate memory for label of size %u.", (uint32_t)labelSize)
		else {
			if(in_pMemoryBuffer) {
				memcpy(a, &in_pMemoryBuffer[totalBytes], labelSize*sizeof(char)); 
				*this								= glabel((const char*)a.Handle, labelSize);
			}
			else 
				*this								= {};
		}
	}
	return totalBytes					+= labelSize;
}

::nwol::error_t						nwol::glabel::save				(FILE* out_pMemoryBuffer)				const													{
	const sint32							labelSize						= (int32_t)size();
	nwol_necall(labelSize.write(out_pMemoryBuffer), "%s", "Failed to write label to file! Label: '%s'.", begin());
	if(labelSize) {
		reterr_error_if(labelSize != (int32_t)fwrite(begin(), sizeof(char), labelSize, out_pMemoryBuffer), "Failed to write label to file! Label: '%s'.", begin());
	}
	return 0;
}

::nwol::error_t						nwol::glabel::load				(FILE* in_pMemoryBuffer)																		{
	sint32									labelSize						= sint32(0);
	nwol_necall(labelSize.read(in_pMemoryBuffer), "%s", "Failed to read label from file!");
	if(labelSize) {
		::nwol::auto_nwol_free					a;
		a.Handle							= (char*)::nwol::nwol_malloc(labelSize);
		reterr_error_if(0 == a, "Failed to allocate memory for label of size %u.", (uint32_t)labelSize);
		if(labelSize != (int32_t)fread(a, sizeof(char), labelSize, in_pMemoryBuffer), "%s", "Failed to read label from file!") {
			error_printf("Failed to read from file label of size: %u bytes.", labelSize);
			*this								= {};
			return -1;
		}
		else
			*this								= glabel((const char*)a.Handle, labelSize);
	}
	return 0;
}