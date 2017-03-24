#include "nwol_array.h"
#include "log.h"
#include "nwol_error.h"

#if defined(__WINDOWS__)
#include <Windows.h>
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

extern "C"
{
	// Dynamically Loaded Application Interface
	const char*										appTitle							();
	uint8_t											appVersion							();

	int32_t											appCreate							(void**	customApplication, ::nwol::SRuntimeValues* runtimeValues);
	int32_t											appDelete							(void**	customApplication);

	int32_t											appSetup							(void*	customApplication);
	int32_t											appCleanup							(void*	customApplication);

	int32_t											appRender							(void*	customApplication);
	::nwol::APPLICATION_STATE						appUpdate							(void*	customApplication, bool requestedExit);

	// --------
	typedef const char*								(*NWOL_RT_CALLBACK_appTitle		)	();
	typedef uint8_t									(*NWOL_RT_CALLBACK_appVersion	)	();

	typedef int32_t									(*NWOL_RT_CALLBACK_appCreate	)	(void**, ::nwol::SRuntimeValues* runtimeValues);
	typedef int32_t									(*NWOL_RT_CALLBACK_appDelete	)	(void**);

	typedef int32_t									(*NWOL_RT_CALLBACK_appSetup		)	(void*);
	typedef int32_t									(*NWOL_RT_CALLBACK_appCleanup	)	(void*);

	typedef int32_t									(*NWOL_RT_CALLBACK_appRender	)	(void*);
	typedef ::nwol::APPLICATION_STATE				(*NWOL_RT_CALLBACK_appUpdate	)	(void*, bool requestedExit);
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
		NWOL_RT_CALLBACK_appTitle							FunctionTitle				= 0;
		NWOL_RT_CALLBACK_appVersion							FunctionVersion				= 0;
		NWOL_RT_CALLBACK_appCreate							FunctionCreate				= 0;
		NWOL_RT_CALLBACK_appDelete							FunctionDelete				= 0;
		NWOL_RT_CALLBACK_appSetup 							FunctionSetup				= 0;
		NWOL_RT_CALLBACK_appCleanup							FunctionCleanup				= 0;
		NWOL_RT_CALLBACK_appRender							FunctionRender				= 0;
		NWOL_RT_CALLBACK_appUpdate							FunctionUpdate				= 0;

		const char_t										* ModuleFile				= 0;
		const char_t										* ModuleTitle				= 0;

		void												* ClientInstance			= 0;
															  
		::nwol::SRuntimeValues								* RuntimeValues				= 0;

		void												* ModuleLibrary				= 0;	/// pointer to dll

		const char_t*										Title						()						const		{ retval_msg_if(nullptr	,							0 == FunctionTitle		, formatModuleFunctionPtrNull, "appTitle"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionTitle		();									}
		uint8_t												Version						()						const		{ retval_msg_if(0xFF	,							0 == FunctionVersion	, formatModuleFunctionPtrNull, "appVersion"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionVersion	();									}
		uint8_t												VersionMajor				()						const		{ const uint8_t version = Version(); return (uint8_t)((version & 0xF0) >> 4);	}
		uint8_t												VersionMinor				()						const		{ const uint8_t version = Version(); return (uint8_t)(version  & 0x0F);			}
		::nwol::error_t										Create						()									{ retval_msg_if(-1,									0 == FunctionCreate		, formatModuleFunctionPtrNull, "appCreate"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionCreate	(&ClientInstance, RuntimeValues);	}
		::nwol::error_t										Delete						()									{ retval_msg_if(-1,									0 == FunctionDelete		, formatModuleFunctionPtrNull, "appDelete"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionDelete	(&ClientInstance);					}
		::nwol::error_t										Setup						()						const		{ retval_msg_if(-1,									0 == FunctionSetup		, formatModuleFunctionPtrNull, "appSetup"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionSetup		(ClientInstance);	}
		::nwol::error_t										Cleanup						()						const		{ retval_msg_if(-1,									0 == FunctionCleanup	, formatModuleFunctionPtrNull, "appCleanup"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionCleanup	(ClientInstance	);					}
		::nwol::error_t										Render						()						const		{ retval_msg_if(-1,									0 == FunctionRender		, formatModuleFunctionPtrNull, "appRender"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionRender	(ClientInstance	);					}
		::nwol::APPLICATION_STATE							Update						(bool bRequestedExit)	const		{ retval_msg_if(::nwol::APPLICATION_STATE_FATAL,	0 == FunctionUpdate		, formatModuleFunctionPtrNull, "appUpdate"	, ModuleTitle ? ModuleTitle : ModuleFile); return FunctionUpdate	(ClientInstance, bRequestedExit);	}
		
		::nwol::RUNTIME_CALLBACK_ID							TestForNullPointerFunctions	()						const		{
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
#endif
	};

	struct SRuntimeValues {	
		SRuntimeValuesDetail								PlatformValues						= {};
		const char_t*										CommandLine							= 0;
		uint32_t											CommandLineArgCount					= 0;
		const char_t*										CommandLineArgList[64]				= {};
		const char_t*										FileNameApplication					= 0;
		const char_t*										FileNameRuntime						= 0;
		::nwol::array_pod<::nwol::SModuleInterface>			Modules								= {};

		// Returns -1 on error or 
		::nwol::error_t										ModuleLoad							(const char_t* moduleName)	{ 
			for(uint32_t i=0; i<Modules.size(); ++i) 
				if(0 == strcmp(Modules[i].ModuleFile, moduleName)) 
					return i; 

			::nwol::SModuleInterface								moduleInstance; 
			::nwol::error_t											errMy								= loadModule(moduleInstance, moduleName); 
			reterr_msg_if_error(errMy, "Module not loaded: %s.", moduleName); 
			return Modules.push_back(moduleInstance); 
		}

	};	// struct
}	// namespace



#endif // __RUNTIME_H__29834908347__
