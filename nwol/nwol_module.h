#include "nwol_error.h"
#include "nwol_log.h"
#include "member_registry.h"

#if defined(__WINDOWS__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#error "Not implemented."
#endif

#ifndef NWOL_MODULE_H__29348729374234
#define NWOL_MODULE_H__29348729374234

namespace nwol {
	struct SModuleBase {
		void										* Handle										= nullptr;
	};

	template <typename _tModule> 
	::nwol::error_t									unloadModule									(_tModule& moduleInstance)								{
		retwarn_error_if(0 == moduleInstance.Handle, "Invalid module handle! This could happen if the unloadModule() function was called twice for the same module.");
		void												* moduleHandle									= moduleInstance.Handle;
		moduleInstance.Handle							= 0;
#if defined(__WINDOWS__)
		FreeLibrary((HMODULE)moduleHandle);
#else
		dlclose(moduleHandle);
#endif
		return 0;
	}

	template <typename _tModule> 
	::nwol::error_t									loadModule										(const char* moduleName, _tModule& moduleInstance)		{
#if defined(__WINDOWS__)
		static	const char_t								* errorFormat0									= "Failed to load library: %s: 0x%X - \"%s\"";
		::std::string										errorString;
		DWORD												lastError;
		reterr_error_if(nullptr == (moduleInstance.Handle = LoadLibrary( moduleName )), errorFormat0, moduleName, (lastError = ::GetLastError()), (errorString = getWindowsErrorAsString(::GetLastError())).c_str()); 

#	define LOAD_FUNCTION_ADDRESS(hModule, lpProcName)	GetProcAddress((HMODULE)hModule, lpProcName)
#else
		static	const char_t								* errorFormat0									= "Failed to load library: %s.";
		reterr_error_if(0 == (moduleInstance.Handle = dlopen( moduleName, RTLD_NOW | RTLD_LOCAL )), errorFormat0, moduleName); 

#	define LOAD_FUNCTION_ADDRESS						dlsym
#endif		

		_tModule::TRegistry&							& memberRegistry									= NWOM_GET_MEMBER_REGISTRY(_tModule); 
		for(uint32_t iSymbol = 0, symbolCount = typeRegistry.Types.size(); iSymbol < symbolCount; ++iSymbol) {
			error_if(0 == (&moduleInstance.Handle)[iSymbol+1]	= LOAD_FUNCTION_ADDRESS(typeRegistry.Types[iSymbol].Name), "Function address not found:" );
		}
		return 0;
	}

#define NWOL_BEGIN_MODULE(	moduleNameSpace, moduleName, moduleHumanName, moduleDescription	)		struct moduleName : public SModuleBase {
#define NWOL_END_MODULE(	moduleNameSpace, moduleName, moduleHumanName, moduleDescription	)		};											\
																																				\

}	// namespace

#endif // NWOL_MODULE_H__29348729374234
