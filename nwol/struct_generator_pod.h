/// Copyright 2017 - asm128
#include "nwol_type_identifier.h"
#include "nwol_datatype.h"
#include "nwol_array.h"

#ifndef NWOL_STRUCT_GENERATOR_POD_H_9287349284928734
#define NWOL_STRUCT_GENERATOR_POD_H_9287349284928734

namespace nwol
{
	struct SMember {
								::nwol::STypeIdentifier										Type		;
								::nwol::gsyslabel											Name
									,														DisplayName
									,														Description
									;
								::nwol::GDATA_TYPE											DataTypeId	;

								const bool													operator==							(const SMember& right)							const	noexcept	{ 
			return	Type		== right.Type
				&&	Name		== right.Name
				&&	DisplayName == right.DisplayName
				&&	Description == right.Description
				&&	DataTypeId	== right.DataTypeId
				;
		}
	};	// struct
}	// namespace

#endif // NWOL_STRUCT_GENERATOR_POD_H_9287349284928734
