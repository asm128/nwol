#include "runtime.h"
#include "label.h"

#if defined(__WINDOWS__)
#elif defined(__ANDROID__) || defined(__LINUX__)
#include <dlfcn.h>
#endif

void											nwol::printErasedModuleInterfacePointers		(::nwol::RUNTIME_CALLBACK_ID erasedCallbacks, const char_t* errorFormat)															{
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_TITLE		), errorFormat, "nwol_moduleTitle"		);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_VERSION	), errorFormat, "nwol_moduleVersion"	);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_CREATE		), errorFormat, "nwol_moduleCreate"		);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_DELETE		), errorFormat, "nwol_moduleDelete"		);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_SETUP		), errorFormat, "nwol_moduleSetup"		);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_CLEANUP	), errorFormat, "nwol_moduleCleanup"	);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_RENDER		), errorFormat, "nwol_moduleRender"		);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_UPDATE		), errorFormat, "nwol_moduleUpdate"		);
}

::nwol::error_t									nwol::unloadModule								(::nwol::SModuleInterface& containerForCallbacks)																					{
	retwarn_error_if(0 == containerForCallbacks.ModuleLibrary, "Invalid module handle! This could happen if the unloadModule() function was called twice for the same module.");													
	void												* moduleHandle									= containerForCallbacks.ModuleLibrary;
	containerForCallbacks.ModuleLibrary				= 0;
	NWOL_PLATFORM_FREE_MODULE(moduleHandle);
	return 0;
}

::nwol::error_t									nwol::loadModule								(::nwol::SModuleInterface& containerForCallbacks, const char_t* moduleName)															{
	reterr_error_if(0 == (containerForCallbacks.ModuleLibrary = NWOL_PLATFORM_LOAD_MODULE(moduleName)), "Failed to load library: %s.", moduleName); 
	containerForCallbacks.FunctionTitle				= (NWOL_RT_CALLBACK_moduleTitle		) NWOL_PLATFORM_LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleTitle"		);
	containerForCallbacks.FunctionVersion			= (NWOL_RT_CALLBACK_moduleVersion	) NWOL_PLATFORM_LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleVersion"	);
	containerForCallbacks.FunctionCreate			= (NWOL_RT_CALLBACK_moduleCreate	) NWOL_PLATFORM_LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleCreate"		);
	containerForCallbacks.FunctionDelete			= (NWOL_RT_CALLBACK_moduleDelete	) NWOL_PLATFORM_LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleDelete"		);
	containerForCallbacks.FunctionSetup				= (NWOL_RT_CALLBACK_moduleSetup		) NWOL_PLATFORM_LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleSetup"		);
	containerForCallbacks.FunctionCleanup			= (NWOL_RT_CALLBACK_moduleCleanup	) NWOL_PLATFORM_LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleCleanup"	);
	containerForCallbacks.FunctionRender			= (NWOL_RT_CALLBACK_moduleRender	) NWOL_PLATFORM_LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleRender"		);
	containerForCallbacks.FunctionUpdate			= (NWOL_RT_CALLBACK_moduleUpdate	) NWOL_PLATFORM_LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleUpdate"		);

	::nwol::RUNTIME_CALLBACK_ID							callbackPointersErased							= containerForCallbacks.TestForNullPointerFunctions();
	if(callbackPointersErased) { 
		::nwol::unloadModule(containerForCallbacks);
		static	const char_t								* errorFormat1									= "Failed to get dynamically loaded function: %s().";
		::nwol::printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
		return -1;
	}
	containerForCallbacks.ModuleFile				= ::nwol::glabel(moduleName, 1024U).c_str();
	containerForCallbacks.ModuleTitle				= ::nwol::glabel(containerForCallbacks.FunctionTitle(), 1024U).c_str();
	return 0;
}

::nwol::error_t									nwol::applicationModuleLoad								(::nwol::SRuntimeValues& runtimeValues, ::nwol::SApplicationModule& containerForCallbacks, const char_t* moduleName)		{
	nwol_necall(::nwol::loadModule(moduleName, containerForCallbacks), "Failed to load application module: %s.", moduleName);
	containerForCallbacks.RuntimeValues				= &runtimeValues;
	containerForCallbacks.ModuleFile				= ::nwol::glabel(moduleName, 1024U).c_str();
	containerForCallbacks.ModuleTitle				= ::nwol::glabel(containerForCallbacks.Title(), 1024U).c_str();
	return 0;
}