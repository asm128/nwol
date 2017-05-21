#pragma warning(disable:4005)
#include "label.h"

#include "label_manager.h"
#include "stype.h"

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

						nwol::gsyslabel::gsyslabel		(const char* label, uint32_t size)																{
	LabelManager							= getSystemLabelManager();
	if( LabelManager->AddLabel(label, size, *this) ) {
		error_printf("Failed to store label!");
	}
}

						nwol::glabel::glabel			(const char* label, uint32_t size)							: LabelManager(getLabelManager())	{
	if( LabelManager->AddLabel(label, size, *this) ) {
		error_printf("Failed to store label!");
	}
}

bool					nwol::glabel::operator	==		(const nwol::glabel& other)				const	noexcept										{ 
		 if(Data == other.Data					)	return true;
	else if(LabelManager == other.LabelManager	)	return false;
	else if(Count != other.Count				)	return false;
	else											return 0 == memcmp(Data, other.Data, Count);

};

uint32_t				nwol::glabel::save				(char* out_pMemoryBuffer)				const													{
	uint32_t								totalBytes			= 0;
	::nwol::sint32							labelSize			= (int32_t)size();
	totalBytes							+= labelSize.write(out_pMemoryBuffer);
	if(labelSize) {
		if(out_pMemoryBuffer)
			memcpy(&out_pMemoryBuffer[totalBytes], begin(), labelSize*sizeof(char)); 
	}
	return totalBytes += labelSize;
}

uint32_t				nwol::glabel::load				(const char* in_pMemoryBuffer)																	{
	uint32_t								totalBytes			= 0;
	sint32									labelSize			= 0;
	totalBytes							+= labelSize.read(in_pMemoryBuffer);
	if(labelSize) {
		char									* a					= (char*)::nwol::nwol_malloc(labelSize);
		if(a) {
			if(in_pMemoryBuffer)
				memcpy(a, &in_pMemoryBuffer[totalBytes], labelSize*sizeof(char)); 

			*this								= glabel(&a[0], labelSize);
			::nwol::nwol_free(a);
		}
		else
			error_printf("Failed to allocate memory for label of size %u.", (uint32_t)labelSize);
	}
	return totalBytes					+= labelSize;
}

::nwol::error_t			nwol::glabel::save				(FILE* out_pMemoryBuffer)				const													{
	const sint32							labelSize			= (int32_t)size();
	uint32_t								errMy				= labelSize.write(out_pMemoryBuffer);
	reterr_error_if_errored(errMy, "%s", "Failed to write label to file! Label: '%s'.", begin());
	if(labelSize) {
		reterr_error_if(labelSize != (int32_t)fwrite(begin(), sizeof(char), labelSize, out_pMemoryBuffer), "Failed to write label to file! Label: '%s'.", begin());
	}
	return 0;
}

::nwol::error_t			nwol::glabel::load				(FILE* in_pMemoryBuffer)																		{
	sint32									labelSize			= sint32(0);
	uint32_t								errMy				= labelSize.read(in_pMemoryBuffer);
	reterr_error_if_errored(errMy, "%s", "Failed to read label from file!");
	if(labelSize) {
		char*									a					= (char*)::nwol::nwol_malloc(labelSize);
		reterr_error_if(0 == a, "Failed to allocate memory for label of size %u.", (uint32_t)labelSize);

		if(labelSize != (int32_t)fread(a, sizeof(char), labelSize, in_pMemoryBuffer)) {
			error_printf("%s", "Failed to read label from file!");
		}
		else
			*this								= glabel(&a[0], labelSize);
		::nwol::nwol_free(a);
	}
	return 0;
}