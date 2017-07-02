#include "nwol_runtime.h"

#ifndef NWOR_H_02973409823749082374923784293
#define NWOR_H_02973409823749082374923784293

namespace nwor 
{
	struct SRuntimeState {	
	#if defined(__WINDOWS__) || defined(__LINUX__) || defined(__ANDROID__)
		::nwol::refcount_t											RenderThreadUsers						= 0			;	// This is used to know if the render thread is still running.
	#else
		::nwol::refcount_t											RenderThreadUsers						;			
	#endif
		::nwol::SApplicationModule									MainModule								= {}		;	// 
		::nwol::SRuntimeValues										RuntimeValues							= {}		;	// 
		bool														Quit									= false		;	// turn to true when quit has been requested
		bool														Processing								= true		;	// 
	};

	int															rtMain									(::nwor::SRuntimeState& runtimeState);
}

#endif // NWOR_H_02973409823749082374923784293