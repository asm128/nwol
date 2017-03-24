#include "graph.h"


#ifndef __GNODE_H__6234462434185144928413598__
#define __GNODE_H__6234462434185144928413598__

namespace nwol
{
	class gnode {
										CGraph*				pGraph					= nullptr;
										::nwol::id_t		Id						= -1;
#if defined(DEBUG) || defined(_DEBUG)
										::nwol::glabel		DebugLabel;	// This provides easier debugging by carrying along the node label.
#endif
	public:
															gnode					(CGraph* _pGraph, const ::nwol::glabel& name="Unnamed");
															gnode					()																																		= default;

		inline							operator			::nwol::id_t			()																																		const	noexcept	{ return Id; }

										::nwol::error_t		getLabel				(::nwol::glabel& output)																												const;
										::nwol::error_t		getAttributeLabel		(::nwol::id_t Index, ::nwol::DATA_DIRECTION direction, ::nwol::glabel& label)															const;
										::nwol::error_t		getAttributeType		(::nwol::id_t Index, ::nwol::DATA_DIRECTION direction, ::nwol::glabel& label)															const;
										::nwol::error_t		getAttributes			(::nwol::DATA_DIRECTION direction, ::nwol::gaindex32& attributes)																		const;
		template<typename _TRef> inline	::nwol::error_t		getAttribute			(::nwol::id_t Index, ::nwol::DATA_DIRECTION direction, ::nwol::gptr_nco<_TRef>& attributeStruct)										const				{ reterr_error_if(0 == pGraph, "Uninitialized node."); return pGraph->GetAttribute	(Id, Index, direction, attributeStruct);				}
		template<typename _TRef> inline	::nwol::error_t		pushAttribute			(::nwol::DATA_DIRECTION direction, ::nwol::gptr_nco<_TRef>& attributeStruct, ::nwol::id_t* index, const ::nwol::glabel& description="")	const				{ reterr_error_if(0 == pGraph, "Uninitialized node."); return pGraph->PushAttribute	(Id, direction, attributeStruct, index, description);	}
		inline							::nwol::error_t		getInputAttributes		(::nwol::gaindex32& attributes)																											const				{ return getAttributes	(::nwol::DATA_DIRECTION_INPUT, attributes);																			}
		inline							::nwol::error_t		getOutputAttributes		(::nwol::gaindex32& attributes)																											const				{ return getAttributes	(::nwol::DATA_DIRECTION_OUTPUT, attributes);																		}
		template<typename _TRef> inline	::nwol::error_t		getInputAttribute		(id_t index, ::nwol::gptr_nco<_TRef>& attributeStruct)																					const				{ return getAttribute	(index, ::nwol::DATA_DIRECTION_INPUT, attributeStruct);																}
		template<typename _TRef> inline	::nwol::error_t		getOutputAttribute		(id_t index, ::nwol::gptr_nco<_TRef>& attributeStruct)																					const				{ return getAttribute	(index, ::nwol::DATA_DIRECTION_OUTPUT, attributeStruct);															}
		template<typename _TRef> inline	::nwol::error_t		pushInputAttribute		(::nwol::gptr_nco<_TRef>& attributeStruct, id_t* index, const ::nwol::glabel& description="")											const				{ return pushAttribute	(::nwol::DATA_DIRECTION_INPUT, attributeStruct, index, description);												}
		template<typename _TRef> inline	::nwol::error_t		pushOutputAttribute		(::nwol::gptr_nco<_TRef>& attributeStruct, id_t* index, const ::nwol::glabel& description="")											const				{ return pushAttribute	(::nwol::DATA_DIRECTION_OUTPUT, attributeStruct, index, description);												}
	};

} // namespace

#endif // __GNODE_H__6234462434185144928413598__