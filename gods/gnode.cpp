#include "gnode.h"

									::gods::gnode::gnode					(CGraph* _pGraph, const ::nwol::glabel& name)
	: pGraph		(_pGraph)
	, Id			(-1)
#if defined(NWOL_DEBUG_ENABLED)
	, DebugLabel	(name)
#endif
{
		 throw_if(0 == _pGraph, "", "Failed to initialize node! Graph pointer is NULL. Node name: '%s'.", name.begin())
	else throw_if(0 > _pGraph->CreateNode(&Id, name), "", "Failed to create node! Node name: '%s'."		, name.begin());
}

::nwol::error_t						gods::gnode::getLabel					(::nwol::glabel& output)												const	{ 
	ree_if(0 == pGraph, "Uninitialized node."); 
#if defined(NWOL_DEBUG_ENABLED)
	::nwol::error_t							errMy									= pGraph->GetNodeLabel(Id, output);
	::nwol::glabel							* _label								= const_cast<::nwol::glabel*>(&DebugLabel);
	*_label								= output;
	return errMy;
#else
	return pGraph->GetNodeLabel(Id, output);
#endif
}

::nwol::error_t						gods::gnode::getAttributes				(::gods::DATA_DIRECTION direction, ::nwol::gaindex32& attributes)				const	{ ree_if(0 == pGraph, "%s", "Uninitialized node."); return pGraph->GetNodeAttributes(Id, direction, attributes); }
::nwol::error_t						gods::gnode::getAttributeLabel			(::nwol::id_t index, ::gods::DATA_DIRECTION direction, ::nwol::glabel& label)	const	{ ree_if(0 == pGraph, "%s", "Uninitialized node."); ::nwol::gaindex32 attributes; nwol_necall(pGraph->GetNodeAttributes(Id, direction, attributes), "Failed to get node attributes. Node Id: '%u'.", (uint32_t)Id); return pGraph->GetAttributeLabel	(attributes[index], label); }
::nwol::error_t						gods::gnode::getAttributeType			(::nwol::id_t index, ::gods::DATA_DIRECTION direction, ::nwol::glabel& label)	const	{ ree_if(0 == pGraph, "%s", "Uninitialized node."); ::nwol::gaindex32 attributes; nwol_necall(pGraph->GetNodeAttributes(Id, direction, attributes), "Failed to get node attributes. Node Id: '%u'.", (uint32_t)Id); return pGraph->GetAttributeType		(attributes[index], label); }
