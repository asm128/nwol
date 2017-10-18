#include "nwol_array.h"
#include "nwol_coord.h"
#include "nwol_ptr.h"

#ifndef NWOL_JSON_H_92749028348923
#define NWOL_JSON_H_92749028348923

namespace nwol 
{
#pragma pack(push, 1)
	enum JSON_OBJECT_TYPE : int32_t
		{	JSON_OBJECT_TYPE_UNKNOWN		= 0
		,	JSON_OBJECT_TYPE_INT	
		,	JSON_OBJECT_TYPE_FLOAT	
		,	JSON_OBJECT_TYPE_STRING	
		,	JSON_OBJECT_TYPE_OBJECT	
		,	JSON_OBJECT_TYPE_ARRAY	
		,	JSON_OBJECT_TYPE_COUNT		
		,	JSON_OBJECT_TYPE_INVALID		= -1
		};

	struct SJSONObject {
						int32_t											ParentIndex;
						JSON_OBJECT_TYPE								Type;
						::nwol::SSpan<uint32_t>							Span;
	};

	struct SJSONNode {
						SJSONObject										* Object;
						SJSONNode										* Parent;
						::nwol::array_obj<::nwol::ptr_obj<SJSONNode>>	Children;
	};

	struct SJSONDocument {
						::nwol::array_obj<::nwol::SJSONObject>			Object;
	};

					::nwol::error_t									jsonRead							(::nwol::SJSONDocument& document, ::nwol::SJSONNode& jsonTree, const char* jsonAsString, uint32_t jsonLength);
	static inline	::nwol::error_t									jsonRead							(::nwol::SJSONDocument& document, ::nwol::SJSONNode& jsonTree, const ::nwol::view_const_string& jsonAsString)			{ return jsonRead(document, jsonTree, jsonAsString.begin(), jsonAsString.size());			}
	static inline	::nwol::error_t									jsonRead							(::nwol::SJSONDocument& document, ::nwol::SJSONNode& jsonTree, const ::std::string& jsonAsString)						{ return jsonRead(document, jsonTree, jsonAsString.data (), (uint32_t)jsonAsString.size());	}
#pragma pack(pop)
}

#endif // NWOL_JSON_H_92749028348923
