#include "type_identifier.h"
#include "datatype.h"
#include "nwol_array.h"

#ifndef __STRUCT_GENERATOR_POD_H__9287349284928734__
#define __STRUCT_GENERATOR_POD_H__9287349284928734__

namespace nwol
{
	struct SMember {
								STypeIdentifier												Type		;
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

#endif // __STRUCT_GENERATOR_POD_H__9287349284928734__
