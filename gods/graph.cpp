#include "graph.h"
#include "gref_definition.h"
#include "stype.h"

GDEFINE_OBJ(nwol, CGraph)
GDEFINE_OBJ(nwol, CGraphNode)

::nwol::error_t			nwol::CGraph::CreateNode				(::nwol::id_t* index, const ::nwol::glabel& nodeLabel)			{
	GPtrObj(CGraphNode)						newNode						= CGraphNode();
	uint32_t								newIndex					= (uint32_t)NodeInstances.size();
	NodeInstances.push(newNode);
	NodeLabels.push_back(nodeLabel);

	SNodeRelationship						nodeParentship				= {-1, -1, NR_CHILD};

	// The following swap provides safety on write access.
	TParentshipRegistry						newNodeParentshipRegistry	= NodeParentshipRegistry;
	newNodeParentshipRegistry.push(nodeParentship);
	NodeParentshipRegistry				= newNodeParentshipRegistry;
	if(index)
		*index								= newIndex;
	return 0;
}

uint32_t				nwol::CGraph::Load						(const char* in_pMemoryBuffer)									{
	uint32_t								totalBytes					= 0;

	// Load node attributes
	::nwol::suint32							nodeCount;
	totalBytes							+= nodeCount.read(in_pMemoryBuffer);
	NodeInstances.resize(nodeCount);
	NodeInstances.create_all();
	for(uint32_t i=0; i<nodeCount; ++i)
		for(uint32_t direction=0; direction < 2; ++direction)
			totalBytes							+= NodeInstances[i]->Attributes[direction].load(&in_pMemoryBuffer[totalBytes]);

	// Load labels
	NodeLabels.resize(nodeCount);
	for(uint32_t i=0; i<nodeCount; i++) {
		::nwol::glabel							label						= NodeLabels[i];
		totalBytes							+= label.load(&in_pMemoryBuffer[totalBytes]);
		NodeLabels[i]						= label;
	}
	for( uint32_t i=0; i<2; ++i)
		totalBytes							+= Attributes[i].Load(&in_pMemoryBuffer[totalBytes]);

	return totalBytes;
}

uint32_t				nwol::CGraph::Save						(char* out_pMemoryBuffer) const									{
	uint32_t								totalBytes					= 0;

	// Save node attributes
	::nwol::suint32							nodeCount					= (uint32_t)NodeInstances.size();
	totalBytes							+= nodeCount.write(&out_pMemoryBuffer[0]);
	for(uint32_t i=0; i<nodeCount; ++i)
		for(uint32_t direction=0; direction < 2; ++direction)
			totalBytes							+= NodeInstances[i]->Attributes[direction].save(out_pMemoryBuffer ? &out_pMemoryBuffer[totalBytes] : 0);

	// Save labels
	for(uint32_t i=0; i<nodeCount; i++) {
		const ::nwol::glabel					label						= NodeLabels[i];
		totalBytes							+= label.save(out_pMemoryBuffer ? &out_pMemoryBuffer[totalBytes] : 0);
	}
	for( uint32_t i=0; i<2; ++i)
		totalBytes							+= Attributes[i].Save(out_pMemoryBuffer ? &out_pMemoryBuffer[totalBytes] : 0);

	return totalBytes;
}