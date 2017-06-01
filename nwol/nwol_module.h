/// Copyright 2016-2017 - asm128
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

#if defined(__WINDOWS__)
#	define NWOL_PLATFORM_LOAD_MODULE								LoadLibrary
#	define NWOL_PLATFORM_FREE_MODULE(hModule)						FreeLibrary((HMODULE)(hModule));
#	define NWOL_PLATFORM_LOAD_FUNCTION_ADDRESS(hModule, lpProcName)	GetProcAddress((HMODULE)hModule, lpProcName)
#else
#	define NWOL_PLATFORM_LOAD_MODULE(moduleName)					dlopen(moduleName, RTLD_NOW | RTLD_LOCAL)
#	define NWOL_PLATFORM_FREE_MODULE								dlclose
#	define NWOL_PLATFORM_LOAD_FUNCTION_ADDRESS						dlsym
#endif

#pragma pack(push, 1)
	struct SModuleBase {
		::nwol::glabel												FilenameOriginal								;
		::nwol::glabel												FilenameImage									;
		void														* Handle										= nullptr;
	};
#pragma pack(pop)

	template <typename _tModule> 
	::nwol::error_t												moduleUnload									(_tModule& moduleInstance)								{
		void															* moduleHandle									= moduleInstance.Handle;
		retwarn_error_if(0 == moduleHandle, "Invalid module handle! This could happen if the moduleUnload() function was called twice for the same module.");
		moduleInstance.Handle										= 0;
		NWOL_PLATFORM_FREE_MODULE(moduleHandle);
		return 0;
	}

	template <typename _tModule> 
	::nwol::error_t												moduleLoad										(const char* moduleName, _tModule& moduleInstance)		{
		_tModule														newModuleInstance								= {};
		if(moduleInstance.Handle)
			::nwol::moduleUnload(moduleInstance);

		newModuleInstance.FilenameOriginal							= 
		newModuleInstance.FilenameImage								= ::nwol::glabel(moduleName, 2048);

		reterr_error_if(0 == (newModuleInstance.Handle = NWOL_PLATFORM_LOAD_MODULE(moduleName)), "Failed to load library: %s.", moduleName); 

		const _tModule::TRegistry										& memberRegistry								= NWOM_GET_MEMBER_REGISTRY(_tModule); 
		const ::nwol::array_view<const ::nwol::gsyslabel>				& memberNames									= memberRegistry.get_names(); 
		const ::nwol::array_view<const ::nwol::GDATA_TYPE>				& memberTypeIds									= memberRegistry.get_data_type_ids(); 
		uint64_t														myErr											= 0;
		for(uint32_t iSymbol = 0, symbolCount = memberNames.size()-1; iSymbol < symbolCount; ++iSymbol) {
			if(memberTypeIds[iSymbol] == ::nwol::GDATA_TYPE_FUN) {
				if(0 == ((&newModuleInstance.Handle)[iSymbol+1] = NWOL_PLATFORM_LOAD_FUNCTION_ADDRESS(newModuleInstance.Handle, memberNames[iSymbol].c_str()))) {
					error_printf("Function address not found: \"%s\".", memberNames[iSymbol].c_str());
					myErr														|= (1ULL << iSymbol);
				}
			}
		}
		if(myErr) {
			error_printf("Failed to load module symbols for library: \"%s\".", moduleName);
			::nwol::moduleUnload(newModuleInstance);
			return -1;
		} 
		else {
			info_printf("Module loaded successfully: \"%s\".", moduleName);
		}
		moduleInstance												= newModuleInstance;
		return 0;
	}

#define NWOL_BEGIN_MODULE(	moduleNameSpace, moduleName)							struct moduleName : public ::nwol::SModuleBase {	
#define NWOL_END_MODULE(	moduleNameSpace, moduleName, moduleDescription	)		};

#define NWOL_PFUNC(returnTypeNameSpace, returnType, functionName)	NWOM(returnTypeNameSpace, returnType, functionName, ::nwol::GDATA_TYPE_FUN, #functionName, "Function")
	
#define NWOL_MODULE_FUNCTION_NAME(functionName)	NWOL_Func_##functionName

#if defined(__WINDOWS__)
#	define NWOL_STDCALL				__stdcall
#	define NWOL_MODULE_API_EXPORT	__declspec(dllexport)
#	define NWOL_MODULE_API_IMPORT	__declspec(dllimport)
#elif defined(__LINUX__)
#	define NWOL_STDCALL				
#	define NWOL_MODULE_API_EXPORT	__attribute__((visibility("default")))
#	define NWOL_MODULE_API_IMPORT
#else
//  do nothing and hope for the best?
#	define NWOL_STDCALL				
#	define NWOL_MODULE_API_EXPORT
#	define NWOL_MODULE_API_IMPORT
#	pragma warning "Unknown dynamic link import/export semantics."
#endif
	
#define NWOL_DECLARE_MODULE_FUNCTION(functionName, ...) 																	\
	extern "C"	::nwol::error_t		 NWOL_STDCALL		functionName									(__VA_ARGS__);		\
	typedef		::nwol::error_t		(NWOL_STDCALL		* NWOL_MODULE_FUNCTION_NAME(functionName))		(__VA_ARGS__);		\

#define NWOM_PFUNC(structNameSpace, structName, functionName)	NWOM(structNameSpace, structName, , NWOL_MODULE_FUNCTION_NAME(functionName), functionName, ::nwol::GDATA_TYPE_FUN, #functionName, "Function")	= nullptr
}	// namespace

#endif // NWOL_MODULE_H__29348729374234
