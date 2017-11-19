#include "nwol_array.h"
#include "nwol_coord.h"
#include "nwol_ptr.h"

#ifndef NWOL_JSON_H_92749028348923
#define NWOL_JSON_H_92749028348923

namespace nwol 
{
#pragma pack(push, 1)
	enum JSON_TYPE : int32_t
		{	JSON_TYPE_UNKNOWN		= 0
		,	JSON_TYPE_INT	
		,	JSON_TYPE_FLOAT	
		,	JSON_TYPE_STRING	
		,	JSON_TYPE_OBJECT	
		,	JSON_TYPE_ARRAY	
		,	JSON_TYPE_COUNT		
		,	JSON_TYPE_INVALID		= -1
		};

	struct SJSONType {
						int32_t												ParentIndex;
						JSON_TYPE											Type;
						::nwol::SSlice<uint32_t>							Span;
	};

	struct SJSONNode {
						SJSONType											* Object;
						SJSONNode											* Parent;
						::nwol::array_obj<::nwol::ptr_obj<SJSONNode>>		Children;
						int32_t												ObjectIndex;
	};

	struct SJSONDocument {
						::nwol::array_obj<::nwol::SJSONType>				Object;
	};

					::nwol::error_t										jsonParse							(::nwol::SJSONDocument& document, ::nwol::SJSONNode& jsonTree, const char* jsonAsString, uint32_t jsonLength);
	static inline	::nwol::error_t										jsonParse							(::nwol::SJSONDocument& document, ::nwol::SJSONNode& jsonTree, const ::nwol::view_const_string& jsonAsString)			{ return ::nwol::jsonParse(document, jsonTree, jsonAsString.begin(), jsonAsString.size());				}
	static inline	::nwol::error_t										jsonParse							(::nwol::SJSONDocument& document, ::nwol::SJSONNode& jsonTree, const ::std::string& jsonAsString)						{ return ::nwol::jsonParse(document, jsonTree, jsonAsString.data (), (uint32_t)jsonAsString.size());	}
#pragma pack(pop)
}

#endif // NWOL_JSON_H_92749028348923
