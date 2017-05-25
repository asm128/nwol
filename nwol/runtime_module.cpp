#include "runtime.h"
#include "label.h"

#if defined(__WINDOWS__)
#elif defined(__ANDROID__) || defined(__LINUX__)
#include <dlfcn.h>
#endif

void											nwol::printErasedModuleInterfacePointers		(::nwol::RUNTIME_CALLBACK_ID erasedCallbacks, const char_t* errorFormat)		{
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_TITLE		), errorFormat, "nwol_moduleTitle"		);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_VERSION	), errorFormat, "nwol_moduleVersion"	);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_CREATE		), errorFormat, "nwol_moduleCreate"		);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_DELETE		), errorFormat, "nwol_moduleDelete"		);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_SETUP		), errorFormat, "nwol_moduleSetup"		);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_CLEANUP	), errorFormat, "nwol_moduleCleanup"	);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_RENDER		), errorFormat, "nwol_moduleRender"		);
	error_if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_UPDATE		), errorFormat, "nwol_moduleUpdate"		);
}

::nwol::error_t									nwol::unloadModule								(::nwol::SModuleInterface& containerForCallbacks)								{
	retwarn_error_if(0 == containerForCallbacks.ModuleLibrary, "Invalid module handle! This could happen if the unloadModule() function was called twice for the same module.");
	void												* moduleHandle								= containerForCallbacks.ModuleLibrary;
	containerForCallbacks.ModuleLibrary				= 0;
#if defined(__WINDOWS__)
	FreeLibrary((HMODULE)moduleHandle);
#else
	dlclose(moduleHandle);
#endif
	return 0;
}

::nwol::error_t									nwol::loadModule								(::nwol::SModuleInterface& containerForCallbacks, const char_t* moduleName)		{
#if defined(__WINDOWS__)
	static	const char_t								* errorFormat0								= "Failed to load library: %s: 0x%X - \"%s\"";
	::std::string										errorString;
    DWORD												lastError;
	reterr_error_if(nullptr == (containerForCallbacks.ModuleLibrary = LoadLibrary( moduleName )), errorFormat0, moduleName, (lastError = ::GetLastError()), (errorString = ::nwol::getWindowsErrorAsString(::GetLastError())).c_str()); 

#	define LOAD_FUNCTION_ADDRESS(hModule, lpProcName)	GetProcAddress((HMODULE)hModule, lpProcName)
#else
	static	const char_t								* errorFormat0								= "Failed to load library: %s.";
	reterr_error_if(0 == (containerForCallbacks.ModuleLibrary = dlopen( moduleName, RTLD_NOW | RTLD_LOCAL )), errorFormat0, moduleName); 

#	define LOAD_FUNCTION_ADDRESS						dlsym
#endif
	containerForCallbacks.FunctionTitle				= (NWOL_RT_CALLBACK_moduleTitle		) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleTitle"	);
	containerForCallbacks.FunctionVersion			= (NWOL_RT_CALLBACK_moduleVersion	) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleVersion"	);
	containerForCallbacks.FunctionCreate			= (NWOL_RT_CALLBACK_moduleCreate	) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleCreate"	);
	containerForCallbacks.FunctionDelete			= (NWOL_RT_CALLBACK_moduleDelete	) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleDelete"	);
	containerForCallbacks.FunctionSetup				= (NWOL_RT_CALLBACK_moduleSetup		) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleSetup"	);
	containerForCallbacks.FunctionCleanup			= (NWOL_RT_CALLBACK_moduleCleanup	) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleCleanup"	);
	containerForCallbacks.FunctionRender			= (NWOL_RT_CALLBACK_moduleRender	) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleRender"	);
	containerForCallbacks.FunctionUpdate			= (NWOL_RT_CALLBACK_moduleUpdate	) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "nwol_moduleUpdate"	);

	::nwol::RUNTIME_CALLBACK_ID							callbackPointersErased						= containerForCallbacks.TestForNullPointerFunctions();
	if(callbackPointersErased) { 
		::nwol::unloadModule(containerForCallbacks);
		static	const char_t								* errorFormat1								= "Failed to get dynamically loaded function: %s().";
		::nwol::printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
		return -1;
	}
	containerForCallbacks.ModuleFile				= ::nwol::glabel(moduleName, ~0U).c_str();
	containerForCallbacks.ModuleTitle				= ::nwol::glabel(containerForCallbacks.FunctionTitle(), ~0U).c_str();
	return 0;
}
