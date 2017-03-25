#include "runtime.h"
#include "label.h"

#if defined(__WINDOWS__)
#elif defined(__ANDROID__) || defined(__LINUX__)
#include <dlfcn.h>
#endif

void										nwol::printErasedModuleInterfacePointers		(::nwol::RUNTIME_CALLBACK_ID erasedCallbacks, const char_t* errorFormat)		{
	if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_TITLE	))	error_printf(errorFormat, "appTitle()"		);
	if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_VERSION	))	error_printf(errorFormat, "appVersion()"	);
	if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_CREATE	))	error_printf(errorFormat, "appCreate()"		);
	if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_DELETE	))	error_printf(errorFormat, "appDelete()"		);
	if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_SETUP	))	error_printf(errorFormat, "appSetup()"		);
	if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_CLEANUP	))	error_printf(errorFormat, "appCleanup()"	);
	if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_RENDER	))	error_printf(errorFormat, "appRender()"		);
	if(0 == (erasedCallbacks & ::nwol::RUNTIME_CALLBACK_ID_UPDATE	))	error_printf(errorFormat, "appUpdate()"		);
}
#if defined (__WINDOWS__)
std::string GetWindowsErrorAsString(DWORD lastError)
{
    //Get the error message, if any.
    if(lastError == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size >= 2 ? size-2 : size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}
#endif

::nwol::error_t								nwol::loadModule								(::nwol::SModuleInterface& containerForCallbacks, const char_t* moduleName)		{

#if defined(__WINDOWS__)
#	define LOAD_FUNCTION_ADDRESS(hModule, lpProcName)	GetProcAddress((HMODULE)hModule, lpProcName)

	static	const char_t*								errorFormat0								= "Failed to load library: %s: 0x%X - \"%s\"";
	std::string											errorString;
    DWORD												lastError;
	reterr_error_if(0 == (containerForCallbacks.ModuleLibrary = LoadLibrary( moduleName )), errorFormat0, moduleName, (lastError = ::GetLastError()), (errorString = GetWindowsErrorAsString(::GetLastError())).c_str()); 
#else
#	define LOAD_FUNCTION_ADDRESS						dlsym

	static	const char_t*								errorFormat0								= "Failed to load library: %s.";
	reterr_error_if(0 == (containerForCallbacks.ModuleLibrary = dlopen( moduleName, RTLD_NOW | RTLD_LOCAL )), errorFormat0, moduleName); 
#endif

	containerForCallbacks.FunctionTitle				= (NWOL_RT_CALLBACK_appTitle	) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "appTitle"	);
	containerForCallbacks.FunctionVersion			= (NWOL_RT_CALLBACK_appVersion	) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "appVersion"	);
	containerForCallbacks.FunctionCreate			= (NWOL_RT_CALLBACK_appCreate	) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "appCreate"	);
	containerForCallbacks.FunctionDelete			= (NWOL_RT_CALLBACK_appDelete	) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "appDelete"	);
	containerForCallbacks.FunctionSetup				= (NWOL_RT_CALLBACK_appSetup	) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "appSetup"	);
	containerForCallbacks.FunctionCleanup			= (NWOL_RT_CALLBACK_appCleanup	) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "appCleanup"	);
	containerForCallbacks.FunctionRender			= (NWOL_RT_CALLBACK_appRender	) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "appRender"	);
	containerForCallbacks.FunctionUpdate			= (NWOL_RT_CALLBACK_appUpdate	) LOAD_FUNCTION_ADDRESS( containerForCallbacks.ModuleLibrary, "appUpdate"	);

	::nwol::RUNTIME_CALLBACK_ID							callbackPointersErased						= containerForCallbacks.TestForNullPointerFunctions();
	if(callbackPointersErased) { 
		unloadModule(containerForCallbacks);
		static	const char_t*								errorFormat1								= "Failed to get dynamically loaded function: %s.";
		printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
		return -1;
	}

	containerForCallbacks.ModuleFile				= ::nwol::glabel(moduleName, ~0U).c_str();
	containerForCallbacks.ModuleTitle				= ::nwol::glabel(containerForCallbacks.FunctionTitle(), ~0U).c_str();

	return 0;
}


::nwol::error_t								nwol::unloadModule								(::nwol::SModuleInterface& containerForCallbacks)								{
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