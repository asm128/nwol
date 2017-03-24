#include "label.h"

#ifndef __TYPE_IDENTIFIER_H__02397492873498__
#define __TYPE_IDENTIFIER_H__02397492873498__

namespace nwol
{
	struct STypeIdentifier {
								::nwol::gsyslabel										NameSpace						= ::nwol::gsyslabel::statics().empty;
								::nwol::gsyslabel										Name							= ::nwol::gsyslabel::statics().empty;

																						STypeIdentifier					()																							= default;
																						STypeIdentifier					(const ::nwol::gsyslabel& nameSpace, const ::nwol::gsyslabel& name)							: NameSpace(nameSpace), Name(name)								{}

								const bool												operator==						(const STypeIdentifier& right)											const	noexcept	{ return NameSpace == right.NameSpace && Name == right.Name;	}
	};	// struct

} // namespace


#endif // __TYPE_IDENTIFIER_H__02397492873498__
