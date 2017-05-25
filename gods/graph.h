#include "reference_registry.h"
#include "gnode_constants.h"

#include "glist.h"

#ifndef __GGRAPH_H__65646523463425211289347491238419__
#define __GGRAPH_H__65646523463425211289347491238419__

namespace nwol
{
	// 0 input 1 output 
	struct CGraphNode												{ ::nwol::gaindex32			Attributes[2]; };
	GDECLARE_OBJ(CGraphNode)

	GDEFINE_ENUM_TYPE(NODE_RELATIONSHIP, int32_t)
	GDEFINE_ENUM_VALUE_NOPREFIX(NODE_RELATIONSHIP, NR_PARENT	, 0);
	GDEFINE_ENUM_VALUE_NOPREFIX(NODE_RELATIONSHIP, NR_CHILD		, 1);

	class CGraph {
#pragma pack(push, 1)
		struct SAttributeId {
								uint32_t							dir	:  1;
								uint32_t							pos	: 31;

			inline constexpr										SAttributeId				()																										noexcept	: dir(1)							, pos(0x7FFFFFFFL)				{}
			inline constexpr										SAttributeId				(const ::nwol::id_t nodeId)																				noexcept	: dir((nodeId & 0x80000000L)>>31)	, pos(nodeId &  0x7FFFFFFFL)	{}
			inline constexpr										SAttributeId				(const char key, const uint32_t index)																	noexcept	: dir(key)							, pos(index)					{}

			inline				operator							::nwol::id_t				()																								const				{ return (pos & 0x7FFFFFFFL) | (((uint32_t)dir)<<31);				}
		};
		struct SNodeRelationship {
								::nwol::id_t						NodeA				: 8;
								::nwol::id_t						NodeB				: 8;
								NODE_RELATIONSHIP					NodeARelationship	: 8;
		};
		struct SAttributeConnection {
								SAttributeId						AttributeA;
								SAttributeId						AttributeB;
		};
#pragma pack(pop)
		typedef	gbuffer<SNodeRelationship	, GTYPEID_DATA_MAKE(SNodeRelationship)		, ::nwol::GUSAGE_INDEX>		TParentshipRegistry;
		typedef	gbuffer<SAttributeConnection, GTYPEID_DATA_MAKE(SAttributeConnection)	, ::nwol::GUSAGE_INDEX>		TConnectionRegistry;

								TParentshipRegistry					NodeParentshipRegistry;
								TConnectionRegistry					AttributeBindings	[2];
								CReferenceContainer					Attributes			[2];
								GListObj(CGraphNode)				NodeInstances;
								::nwol::array_obj<::nwol::glabel>	NodeLabels;

		inline					::nwol::error_t						validateNodeIndex			(::nwol::id_t nodeIndex)																		const	noexcept	{ return (0 > nodeIndex || ((::nwol::id_t)NodeInstances.size()) <= nodeIndex) ? -1 : 0; }

	public:
								CGraph&								operator=					(const CGraph& other)																								= default;
								uint32_t							Load						(const char* in_pMemoryBuffer);		// Returns the amount of bytes read
								uint32_t							Save						(char* out_pMemoryBuffer)				const;	// Returns the amount of bytes written

								::nwol::error_t						CreateNode					(::nwol::id_t* index, const ::nwol::glabel& nodeLabel);
								::nwol::error_t						GetNodeLabel				(::nwol::id_t nodeIndex		, ::nwol::glabel& output)																{ reterr_error_if(errored(validateNodeIndex(nodeIndex)), "invalid node index: '%u'", (uint32_t)nodeIndex); output		= NodeLabels[nodeIndex];							return 0;	}
								::nwol::error_t						GetNodeAttributes			(::nwol::id_t nodeIndex		, ::nwol::DATA_DIRECTION direction, ::nwol::gaindex32& attributes)						{ reterr_error_if(errored(validateNodeIndex(nodeIndex)), "invalid node index: '%u'", (uint32_t)nodeIndex); attributes	= NodeInstances[nodeIndex]->Attributes[direction];	return 0;	}
								::nwol::error_t						GetAttributeLabel			(::nwol::id_t attributeId	, ::nwol::glabel& output)																{ SAttributeId attrId = attributeId; return Attributes[attrId.dir].GetDescription	(attrId.pos, output);	}
								::nwol::error_t						GetAttributeType			(::nwol::id_t attributeId	, ::nwol::glabel& output)																{ SAttributeId attrId = attributeId; return Attributes[attrId.dir].GetTypeName		(attrId.pos, output);	}
		
		template<typename _tRef>
								::nwol::error_t						GetAttribute				(::nwol::id_t nodeIndex, ::nwol::id_t attributeIndex, ::nwol::DATA_DIRECTION direction, ::nwol::gptr_nco<_tRef>& attributeStruct)										{
			reterr_error_if(validateNodeIndex(nodeIndex), "invalid node index: '%u'", (uint32_t)nodeIndex);

			CGraphNode														* pNode						= NodeInstances[nodeIndex];
			SAttributeId													attrId						= pNode->Attributes[direction][attributeIndex];
			Attributes[attrId.dir].AcquireReference(attrId.pos, attributeStruct);
			return 0;		
		}

		template<typename _tRef>
								::nwol::error_t						PushAttribute				(::nwol::id_t nodeIndex, ::nwol::DATA_DIRECTION direction, ::nwol::gptr_nco<_tRef>& attributeStruct, ::nwol::id_t* _attributeIndex, const ::nwol::glabel& description)	{
			reterr_error_if(validateNodeIndex(nodeIndex), "invalid node index: '%u'", (uint32_t)nodeIndex);
			reterr_error_if(0 == _attributeIndex, "%s", "Output attribute index pointer must be something other than null.");

			::nwol::id_t													attributePos				= -1;
			nwol_ecall(Attributes[direction].PushReference(attributeStruct, &attributePos, description)
				,	"Failed to push attribute reference into attribute registry. Node index: '%u'. Attribute type: '%s'. Attribute description: '%s'."
				,	(uint32_t)	nodeIndex
				,	_tRef::get_type_name().begin()
				,	description.begin()
				);

			CGraphNode														* pNode						= NodeInstances[nodeIndex];
			int32_t															newIndex					= pNode->Attributes[direction].size();
			SAttributeId													newId						= {(const char)direction, (const uint32_t)attributePos};
			nwol_ecall(pNode->Attributes[newId.dir].push_back(newId)
				,	"Failed to push attribute index into node. Node index: '%u'. AttributeId: '0x%X'. Attribute type: '%s'. Attribute description: '%s'."
				,	(uint32_t)		nodeIndex
				,	(::nwol::id_t)	newId
				,	_tRef::get_type_name().begin()
				,	description.begin()
				);

			if(_attributeIndex)
				*_attributeIndex											= newIndex;
			else
				warning_printf("%s", "No output ");
			return newIndex;
		}
	};
	GDECLARE_OBJ(CGraph)
} // namespace

#endif // __GGRAPH_H__65646523463425211289347491238419__
