#include "nwol_input.h"
#include "runtime_impl.h"

namespace nwol {
	struct SModule {
		SRuntimeValues					* RuntimeValues						= nullptr;

										SModule								(::nwol::SRuntimeValues* runtimeValues)		: RuntimeValues(runtimeValues)			{}
	};
}

struct SInput : public ::nwol::SModule {
										SInput								(::nwol::SRuntimeValues* runtimeValues)		: SModule(runtimeValues)			{}
};

int32_t								setup								(::SInput & /*instanceApp*/)						{ return 0;																}
int32_t								update								(::SInput & /*instanceApp*/, bool exitRequested)	{ if(exitRequested) return ::nwol::APPLICATION_STATE_EXIT; return 0;	}
int32_t								render								(::SInput & /*instanceApp*/)						{ return 0;																}
int32_t								cleanup								(::SInput & /*instanceApp*/)						{ return 0;																}

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(::SInput, "Input Device Driver", 0, 1);