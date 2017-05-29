#include "runtime.h"
#include "label.h"

#if defined(__WINDOWS__)
#elif defined(__ANDROID__) || defined(__LINUX__)
#include <dlfcn.h>
#endif

void											nwol::printErasedModuleInterfacePointers		(::nwol::RUNTIME_CALLBACK_ID erasedCallbacks, const char_t* errorFormat)													{
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_TITLE		), errorFormat, "nwol_moduleTitle"		);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_VERSION	), errorFormat, "nwol_moduleVersion"	);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_CREATE		), errorFormat, "nwol_moduleCreate"		);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_DELETE		), errorFormat, "nwol_moduleDelete"		);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_SETUP		), errorFormat, "nwol_moduleSetup"		);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_CLEANUP	), errorFormat, "nwol_moduleCleanup"	);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_RENDER		), errorFormat, "nwol_moduleRender"		);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_UPDATE		), errorFormat, "nwol_moduleUpdate"		);
}

::nwol::error_t									nwol::applicationModuleLoad						(::nwol::SRuntimeValues& runtimeValues, ::nwol::SApplicationModule& containerForCallbacks, const char_t* moduleName)		{
	nwol_necall(::nwol::moduleLoad(moduleName, containerForCallbacks), "Failed to load application module: %s.", moduleName);
	::nwol::RUNTIME_CALLBACK_ID							callbackPointersErased							= containerForCallbacks.TestForNullPointerFunctions();
	if(callbackPointersErased) { 
		error_if(errored(::nwol::moduleUnload(containerForCallbacks)), "The module wasn't loaded.");
		static	const char_t								* errorFormat1									= "Failed to get dynamically loaded function: %s().";
		::nwol::printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
		return -1;
	}
	containerForCallbacks.RuntimeValues				= &runtimeValues;
	//containerForCallbacks.ModuleFile				= ::nwol::glabel(moduleName, 1024U).c_str();
	containerForCallbacks.ModuleTitle				= ::nwol::glabel(containerForCallbacks.Title(), 1024U).c_str();
	return 0;
}