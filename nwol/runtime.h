#include "nwol_array.h"
#include "nwol_error.h"
#include "nwol_screen.h"

#if defined(__WINDOWS__)

#elif defined(__ANDROID__)
#include <android/native_activity.h>
#include <jni.h>
#endif

#ifndef __RUNTIME_H__29834908347__
#define __RUNTIME_H__29834908347__

namespace nwol
{
	enum APPLICATION_STATE : int8_t
		{	APPLICATION_STATE_NORMAL					=  0
		,	APPLICATION_STATE_EXIT						=  1
		,	APPLICATION_STATE_BUSY						=  2
		,	APPLICATION_STATE_STAY						=  3
		,	APPLICATION_STATE_FATAL						= -1
		};

	struct SRuntimeValues;
}	// nwol

#define NWOL_MODULE_EXPORT_NAME(functionToken)		nwol_module##functionToken

extern "C"
{
	// Dynamically Loaded Application Interface
	const char*										nwol_moduleTitle						()																		noexcept;		
	uint8_t											nwol_moduleVersion						()																		noexcept;
	int32_t											nwol_moduleCreate						(void**	customApplication, ::nwol::SRuntimeValues* runtimeValues)		noexcept;
	int32_t											nwol_moduleDelete						(void**	customApplication)												noexcept;
	int32_t											nwol_moduleSetup						(void*	customApplication)												noexcept;
	int32_t											nwol_moduleCleanup						(void*	customApplication)												noexcept;
	int32_t											nwol_moduleRender						(void*	customApplication)												noexcept;
	::nwol::APPLICATION_STATE						nwol_moduleUpdate						(void*	customApplication, bool requestedExit)							noexcept;

	typedef const char*								(*NWOL_RT_CALLBACK_moduleTitle		)	()																		;
	typedef uint8_t									(*NWOL_RT_CALLBACK_moduleVersion	)	()																		;
	typedef int32_t									(*NWOL_RT_CALLBACK_moduleCreate		)	(void** customApplication, ::nwol::SRuntimeValues* runtimeValues)		;
	typedef int32_t									(*NWOL_RT_CALLBACK_moduleDelete		)	(void** customApplication)												;
	typedef int32_t									(*NWOL_RT_CALLBACK_moduleSetup		)	(void*  customApplication)												;
	typedef int32_t									(*NWOL_RT_CALLBACK_moduleCleanup	)	(void*  customApplication)												;
	typedef int32_t									(*NWOL_RT_CALLBACK_moduleRender		)	(void*  customApplication)												;
	typedef ::nwol::APPLICATION_STATE				(*NWOL_RT_CALLBACK_moduleUpdate		)	(void*  customApplication, bool requestedExit)							;
}

namespace nwol
{
	enum RUNTIME_CALLBACK_ID : uint8_t
		{	RUNTIME_CALLBACK_ID_NONE						= 0
		,	RUNTIME_CALLBACK_ID_TITLE						= 0x01
		,	RUNTIME_CALLBACK_ID_VERSION						= 0x02
		,	RUNTIME_CALLBACK_ID_CREATE						= 0x04
		,	RUNTIME_CALLBACK_ID_DELETE						= 0x08
		,	RUNTIME_CALLBACK_ID_SETUP						= 0x10
		,	RUNTIME_CALLBACK_ID_CLEANUP						= 0x20
		,	RUNTIME_CALLBACK_ID_RENDER						= 0x40
		,	RUNTIME_CALLBACK_ID_UPDATE						= 0x80
		};									

	static	const char	formatModuleFunctionPtrNull[] = "Function pointer for %s() in module %s is null.";
	struct SModuleInterface {
		void												* ModuleLibrary				= 0;	/// pointer to dll

		NWOL_RT_CALLBACK_moduleTitle						FunctionTitle				= 0;
		NWOL_RT_CALLBACK_moduleVersion						FunctionVersion				= 0;
		NWOL_RT_CALLBACK_moduleCreate						FunctionCreate				= 0;
		NWOL_RT_CALLBACK_moduleDelete						FunctionDelete				= 0;
		NWOL_RT_CALLBACK_moduleSetup 						FunctionSetup				= 0;
		NWOL_RT_CALLBACK_moduleCleanup						FunctionCleanup				= 0;
		NWOL_RT_CALLBACK_moduleRender						FunctionRender				= 0;
		NWOL_RT_CALLBACK_moduleUpdate						FunctionUpdate				= 0;

		const char_t										* ModuleFile				= 0;
		const char_t										* ModuleTitle				= 0;

		void												* ClientInstance			= 0;
															  
		::nwol::SRuntimeValues								* RuntimeValues				= 0;

		const char_t*										Title						()						const				{ retval_error_if(nullptr	,							0 == FunctionTitle		, formatModuleFunctionPtrNull, "moduleTitle"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionTitle		();									}
		uint8_t												Version						()						const				{ retval_error_if(0xFF		,							0 == FunctionVersion	, formatModuleFunctionPtrNull, "moduleVersion"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionVersion	();									}
		uint8_t												VersionMajor				()						const	noexcept	{ const uint8_t version = Version(); return (uint8_t)((version & 0xF0) >> 4);	}
		uint8_t												VersionMinor				()						const	noexcept	{ const uint8_t version = Version(); return (uint8_t)(version  & 0x0F);			}
		::nwol::error_t										Create						()											{ retval_error_if(-1,									0 == FunctionCreate		, formatModuleFunctionPtrNull, "moduleCreate"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionCreate	(&ClientInstance, RuntimeValues);	}
		::nwol::error_t										Delete						()											{ retval_error_if(-1,									0 == FunctionDelete		, formatModuleFunctionPtrNull, "moduleDelete"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionDelete	(&ClientInstance);					}
		::nwol::error_t										Setup						()						const				{ retval_error_if(-1,									0 == FunctionSetup		, formatModuleFunctionPtrNull, "moduleSetup"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionSetup		(ClientInstance	);					}
		::nwol::error_t										Cleanup						()						const				{ retval_error_if(-1,									0 == FunctionCleanup	, formatModuleFunctionPtrNull, "moduleCleanup"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionCleanup	(ClientInstance	);					}
		::nwol::error_t										Render						()						const				{ retval_error_if(-1,									0 == FunctionRender		, formatModuleFunctionPtrNull, "moduleRender"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionRender	(ClientInstance	);					}
		::nwol::APPLICATION_STATE							Update						(bool bRequestedExit)	const				{ retval_error_if(::nwol::APPLICATION_STATE_FATAL	,	0 == FunctionUpdate		, formatModuleFunctionPtrNull, "moduleUpdate"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionUpdate	(ClientInstance, bRequestedExit);	}
		
		::nwol::RUNTIME_CALLBACK_ID							TestForNullPointerFunctions	()						const				{
			::nwol::RUNTIME_CALLBACK_ID								result						= RUNTIME_CALLBACK_ID_NONE;
			if(0 == FunctionTitle	)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_TITLE		);
			if(0 == FunctionVersion	)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_VERSION	);
			if(0 == FunctionCreate	)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_CREATE		);
			if(0 == FunctionDelete	)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_DELETE		);
			if(0 == FunctionSetup	)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_SETUP		);
			if(0 == FunctionCleanup	)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_CLEANUP	);
			if(0 == FunctionRender	)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_RENDER		);
			if(0 == FunctionUpdate	)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_UPDATE		);
			return result;
		}

	};	// struct

	void												printErasedModuleInterfacePointers	(::nwol::RUNTIME_CALLBACK_ID erasedCallbacks	, const char_t* errorFormat);
	::nwol::error_t										loadModule							(::nwol::SModuleInterface& containerForCallbacks, const char_t* moduleName);
	::nwol::error_t										unloadModule						(::nwol::SModuleInterface& containerForCallbacks);
}	// namespace

namespace nwol
{
	struct SRuntimeValuesDetail {	
#if defined(__ANDROID__)
		ANativeActivity										* activity							= nullptr;
		void												* savedState						= nullptr;
		size_t												savedStateSize						= 0;
#elif defined(__WINDOWS__)
		HINSTANCE											hInstance							= NULL;
		HINSTANCE											hPrevInstance						= NULL;
		LPSTR												lpCmdLine							= NULL;
		int													nShowCmd							= 0;

		// We need to add the window management in this abstraction level because in some systems we can't control the creation of the window and 
		// we require to mimic that situation from the beginning in order to keep platform compatibility.
		WNDCLASSEX											MainWindowClass						= {};
		DWORD												MainWindowStyle						= WS_OVERLAPPED | WS_THICKFRAME | WS_BORDER | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;
#endif
	};

	struct SRuntimeValues {	
		SRuntimeValuesDetail								PlatformDetail						= {};
		const char_t										* CommandLine						= nullptr;
		uint32_t											CommandLineArgCount					= 0;
		const char_t*										CommandLineArgList[64]				= {};
		const char_t										* FileNameApplication				= nullptr;
		const char_t										* FileNameRuntime					= nullptr;
		::nwol::SScreen										Screen								= {};
		::nwol::array_pod<::nwol::SModuleInterface>			Modules								= {};

		// Returns -1 on error or the index of the loaded module.
		::nwol::error_t										ModuleLoad							(const char_t* moduleName)	{
			for(uint32_t i=0; i<Modules.size(); ++i) 
				if(0 == strcmp(Modules[i].ModuleFile, moduleName)) 
					return i; 

			::nwol::SModuleInterface								moduleInstance; 
			::nwol::error_t											errMy								= ::nwol::loadModule(moduleInstance, moduleName); 
			reterr_error_if_errored(errMy, "Module not loaded: %s.", moduleName); 
			return Modules.push_back(moduleInstance); 
		}
	};	// struct
}	// namespace



#endif // __RUNTIME_H__29834908347__
