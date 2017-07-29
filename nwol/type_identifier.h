/// Copyright 2017 - asm128
#include "label.h"

#ifndef NWOL_TYPE_IDENTIFIER_H_02397492873498
#define NWOL_TYPE_IDENTIFIER_H_02397492873498

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


#endif // NWOL_TYPE_IDENTIFIER_H_02397492873498
