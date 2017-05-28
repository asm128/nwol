#include "gnode.h"

									::nwol::gnode::gnode					(CGraph* _pGraph, const ::nwol::glabel& name)
	: pGraph		(_pGraph)
	, Id			(-1)
#if defined(NWOL_DEBUG_ENABLED)
	, DebugLabel	(name)
#endif
{
		 throw_if(0 == _pGraph, "", "Failed to initialize node! Graph pointer is NULL. Node name: '%s'.", name.begin())
	else throw_if(0 > _pGraph->CreateNode(&Id, name), "", "Failed to create node! Node name: '%s'."		, name.begin());
}

::nwol::error_t						nwol::gnode::getLabel					(::nwol::glabel& output)												const	{ 
	reterr_error_if(0 == pGraph, "Uninitialized node."); 
#if defined(NWOL_DEBUG_ENABLED)
	::nwol::error_t							errMy									= pGraph->GetNodeLabel(Id, output);
	::nwol::glabel							* _label								= const_cast<::nwol::glabel*>(&DebugLabel);
	*_label								= output;
	return errMy;
#else
	return pGraph->GetNodeLabel(Id, output);
#endif
}

::nwol::error_t						nwol::gnode::getAttributes				(::nwol::DATA_DIRECTION direction, gaindex32& attributes)				const	{ reterr_error_if(0 == pGraph, "%s", "Uninitialized node."); return pGraph->GetNodeAttributes(Id, direction, attributes); }
::nwol::error_t						nwol::gnode::getAttributeLabel			(id_t index, ::nwol::DATA_DIRECTION direction, ::nwol::glabel& label)	const	{ reterr_error_if(0 == pGraph, "%s", "Uninitialized node."); gaindex32 attributes; nwol_necall(pGraph->GetNodeAttributes(Id, direction, attributes), "Failed to get node attributes. Node Id: '%u'.", (uint32_t)Id); return pGraph->GetAttributeLabel	(attributes[index], label); }
::nwol::error_t						nwol::gnode::getAttributeType			(id_t index, ::nwol::DATA_DIRECTION direction, ::nwol::glabel& label)	const	{ reterr_error_if(0 == pGraph, "%s", "Uninitialized node."); gaindex32 attributes; nwol_necall(pGraph->GetNodeAttributes(Id, direction, attributes), "Failed to get node attributes. Node Id: '%u'.", (uint32_t)Id); return pGraph->GetAttributeType	(attributes[index], label); }
