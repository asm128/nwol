#include "nwol_error.h"
#include "nwol_log.h"
#include "member_registry.h"

#if defined(__WINDOWS__)
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#else
#	error "Not implemented."
#endif

#ifndef NWOL_MODULE_H__29348729374234
#define NWOL_MODULE_H__29348729374234

namespace nwol {
#pragma pack(push, 1)
	struct SModuleBase {
		void										* Handle										= nullptr;
	};
#pragma pack(pop)

	template <typename _tModule> 
	::nwol::error_t									unloadModule									(_tModule& moduleInstance)								{
		void												* moduleHandle									= moduleInstance.Handle;
		retwarn_error_if(0 == moduleHandle, "Invalid module handle! This could happen if the unloadModule() function was called twice for the same module.");
		moduleInstance.Handle							= 0;
#if defined(__WINDOWS__)
		::FreeLibrary((HMODULE)moduleHandle);
#else
		::dlclose(moduleHandle);
#endif
		return 0;
	}

	template <typename _tModule> 
	::nwol::error_t									loadModule										(const char* moduleName, _tModule& moduleInstance)		{
		_tModule											newModuleInstance								= {};
		if(moduleInstance.Handle)
			::nwol::unloadModule(moduleInstance);

#if defined(__WINDOWS__)
		static	const char_t								* errorFormat0									= "Failed to load library: %s: 0x%X - \"%s\"";
		::std::string										errorString;
		DWORD												lastError;
		reterr_error_if(nullptr == (newModuleInstance.Handle = LoadLibrary( moduleName )), errorFormat0, moduleName, (lastError = ::GetLastError()), (errorString = getWindowsErrorAsString(::GetLastError())).c_str()); 

#	define LOAD_FUNCTION_ADDRESS(hModule, lpProcName)	GetProcAddress((HMODULE)hModule, lpProcName)
#else
		static	const char_t								* errorFormat0									= "Failed to load library: %s.";
		reterr_error_if(0 == (newModuleInstance.Handle = dlopen( moduleName, RTLD_NOW | RTLD_LOCAL )), errorFormat0, moduleName); 

#	define LOAD_FUNCTION_ADDRESS							dlsym
#endif		

		const _tModule::TRegistry							& memberRegistry								= NWOM_GET_MEMBER_REGISTRY(_tModule); 
		const ::nwol::array_view<const ::nwol::gsyslabel>	& memberNames									= memberRegistry.get_names(); 
		uint64_t											myErr											= 0;
		for(uint32_t iSymbol = 0, symbolCount = memberNames.size()-1; iSymbol < symbolCount; ++iSymbol) {
			if(0 == ((&newModuleInstance.Handle)[iSymbol+1] = LOAD_FUNCTION_ADDRESS(newModuleInstance.Handle, memberNames[iSymbol].c_str()))) {
				error_printf("Function address not found: \"%s\".", memberNames[iSymbol].c_str());
				myErr											|= (1ULL << iSymbol);
			}
		}

		if(myErr) {
			error_printf("Failed to load module symbols for library: \"%s\".", moduleName);
			::nwol::unloadModule(newModuleInstance);
			return -1;
		} 
		else {
			info_printf("Module loaded successfully: \"%s\".", moduleName);
		}

		moduleInstance									= newModuleInstance;
		return 0;
	}

#define NWOL_BEGIN_MODULE(	moduleNameSpace, moduleName)							struct moduleName : public ::nwol::SModuleBase {	
#define NWOL_END_MODULE(	moduleNameSpace, moduleName, moduleDescription	)		};

#define NWOL_PFUNC(returnTypeNameSpace, returnType, functionName)	NWOM(returnTypeNameSpace, returnType, functionName, ::nwol::GDATA_TYPE_FUN, #functionName, "Function")
	
#define NWOL_MODULE_FUNCTION_NAME(functionName)	NWOL_Func_##functionName

#define NWOL_DECLARE_MODULE_FUNCTION(functionName, ...) 																\
	extern "C"	::nwol::error_t		 _stdcall		functionName									(__VA_ARGS__);		\
	typedef		::nwol::error_t		(_stdcall		* NWOL_MODULE_FUNCTION_NAME(functionName))		(__VA_ARGS__);		\

#define NWOM_PFUNC(functionName)	NWOM(, NWOL_MODULE_FUNCTION_NAME(functionName), functionName, ::nwol::GDATA_TYPE_FUN, #functionName, "Function")	= nullptr

#if defined(__WINDOWS__)
#	define NWOL_MODULE_API_EXPORT __declspec(dllexport)
#	define NWOL_MODULE_API_IMPORT __declspec(dllimport)
#elif defined(__LINUX__)
#	define NWOL_MODULE_API_EXPORT __attribute__((visibility("default")))
#	define NWOL_MODULE_API_IMPORT
#else
//  do nothing and hope for the best?
#	define NWOL_MODULE_API_EXPORT
#	define NWOL_MODULE_API_IMPORT
#	pragma warning "Unknown dynamic link import/export semantics."
#endif

}	// namespace

#endif // NWOL_MODULE_H__29348729374234
