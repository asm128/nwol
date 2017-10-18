#include "graph.h"
#include "gref_definition.h"
#include "stype.h"

GDEFINE_OBJ(gods, CGraph)
GDEFINE_OBJ(gods, CGraphNode)

::nwol::error_t						gods::CGraph::CreateNode				(::nwol::id_t* index, const ::nwol::glabel& nodeLabel)			{
	GPtrObj(CGraphNode)						newNode						= CGraphNode();
	int32_t									newIndex					= (int32_t)NodeInstances.size();
	ree_if(newIndex != NodeInstances.push_back(newNode), "Failed to push node instance. Out of memory?");
	if(newIndex != NodeLabels.push_back(nodeLabel)) {
		error_printf("Failed to push node label. Out of memory?");
		NodeInstances.resize(NodeInstances.size()-1);
		return -1;
	}

	SNodeRelationship						nodeParentship				= {-1, -1, NR_CHILD};
	if(newIndex != NodeParentshipRegistry.push_back(nodeParentship)) { 
		error_printf("Failed to push node parentship. Out of memory?");
		NodeInstances	.resize(NodeInstances	.size()-1);
		NodeLabels		.resize(NodeLabels		.size()-1);
		return -1;
	}
	if(index)
		*index								= newIndex;
	return 0;
}

::nwol::error_t						gods::CGraph::Load						(const char* in_pMemoryBuffer, uint32_t* inout_bytesRead)										{
	uint32_t								totalBytes					= 0;

	// Load node attributes
	::nwol::suint32							nodeCount;
	totalBytes							+= nodeCount.read(in_pMemoryBuffer);
	nwol_necall(NodeInstances.resize(nodeCount), "Failed to resize array.");
	nwol_necall(NodeInstances.create_all(), "Failed to allocate instances! Out of memory?");

	for(uint32_t i=0; i<nodeCount; ++i)
		for(uint32_t direction=0; direction < 2; ++direction)
			totalBytes							+= NodeInstances[i]->Attributes[direction].load(&in_pMemoryBuffer[totalBytes]);

	// Load labels
	nwol_necall(NodeLabels.resize(nodeCount), "Failed to resize label array. Out of memory?");
	for(uint32_t i=0; i<nodeCount; i++) {
		::nwol::glabel							label						= NodeLabels[i];
		totalBytes							+= label.load(&in_pMemoryBuffer[totalBytes]);
		NodeLabels[i]						= label;
	}
	for( uint32_t i = 0; i<2; ++i)
		totalBytes							+= Attributes[i].Load(&in_pMemoryBuffer[totalBytes]);

	safe_assign(inout_bytesRead, totalBytes);
	return totalBytes;
}

::nwol::error_t						gods::CGraph::Save						(char* out_pMemoryBuffer, uint32_t* inout_bytesWritten) const									{
	uint32_t								totalBytes								= 0;

	::nwol::suint32							nodeCount								= (uint32_t)NodeInstances.size();
	totalBytes							+= nodeCount.write(&out_pMemoryBuffer[0]);
	for(uint32_t i = 0; i < nodeCount; ++i)		// Save node attributes
		for(uint32_t direction = 0; direction < 2; ++direction)
			totalBytes							+= NodeInstances[i]->Attributes[direction].save(out_pMemoryBuffer ? &out_pMemoryBuffer[totalBytes] : 0);

	for(uint32_t i = 0; i < nodeCount; ++i) {		// Save labels
		const ::nwol::glabel					label									= NodeLabels[i];
		totalBytes							+= label.save(out_pMemoryBuffer ? &out_pMemoryBuffer[totalBytes] : 0);
	}

	for( uint32_t i = 0; i < 2; ++i)
		totalBytes							+= Attributes[i].Save(out_pMemoryBuffer ? &out_pMemoryBuffer[totalBytes] : 0);

	safe_assign(inout_bytesWritten, totalBytes);
	return totalBytes;
}