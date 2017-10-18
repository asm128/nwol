// These sources are best viewed in Visual Studio 2017 with a screen of at least 1920x1080 screen and the zoom set to 81 %.
// Pablo Ariel Zorrilla Cepeda (asm128) Copyright (c) 2010-2017 - Distributed under the MIT License.
#include "nwol_screen.h"
#include "nwol_module.h"
#include "nwol_enum.h"
#include "nwol_ptr.h"

#if defined(__WINDOWS__)

#elif defined(__ANDROID__)
#include <android/native_activity.h>
#include <jni.h>
#endif

#ifndef RUNTIME_H__29834908347
#define RUNTIME_H__29834908347

namespace nwol {
	struct SRuntimeValues;
}	// nwol

#define NWOL_MODULE_EXPORT_NAME(functionToken)		nwol_module##functionToken

NWOL_DECLARE_MODULE_FUNCTION(nwol_moduleTitle	, char_t* outputBuffer, uint8_t* outputBufferLenght					);
NWOL_DECLARE_MODULE_FUNCTION(nwol_moduleVersion	, uint32_t* version													);
NWOL_DECLARE_MODULE_FUNCTION(nwol_moduleCreate	, void** customApplication, ::nwol::SRuntimeValues* runtimeValues	);
NWOL_DECLARE_MODULE_FUNCTION(nwol_moduleDelete	, void** customApplication											);
NWOL_DECLARE_MODULE_FUNCTION(nwol_moduleSetup	, void*	 customApplication											);
NWOL_DECLARE_MODULE_FUNCTION(nwol_moduleCleanup	, void*	 customApplication											);
NWOL_DECLARE_MODULE_FUNCTION(nwol_moduleRender	, void*	 customApplication											);
NWOL_DECLARE_MODULE_FUNCTION(nwol_moduleUpdate	, void*	 customApplication, bool requestedExit						);

namespace nwol
{
	GDEFINE_ENUM_TYPE(APPLICATION_STATE, uint8_t);
	GDEFINE_ENUM_VALUE(APPLICATION_STATE, NORMAL	,  0);
	GDEFINE_ENUM_VALUE(APPLICATION_STATE, EXIT		,  1);
	GDEFINE_ENUM_VALUE(APPLICATION_STATE, BUSY		,  2);
	GDEFINE_ENUM_VALUE(APPLICATION_STATE, STAY		,  3);

	enum RUNTIME_CALLBACK_ID : uint8_t
		{	RUNTIME_CALLBACK_ID_NONE							= 0
		,	RUNTIME_CALLBACK_ID_TITLE							= 0x01
		,	RUNTIME_CALLBACK_ID_VERSION							= 0x02
		,	RUNTIME_CALLBACK_ID_CREATE							= 0x04
		,	RUNTIME_CALLBACK_ID_DELETE							= 0x08
		,	RUNTIME_CALLBACK_ID_SETUP							= 0x10
		,	RUNTIME_CALLBACK_ID_CLEANUP							= 0x20
		,	RUNTIME_CALLBACK_ID_RENDER							= 0x40
		,	RUNTIME_CALLBACK_ID_UPDATE							= 0x80
		};					

	static constexpr	const char								formatModuleFunctionPtrNull[]	= "Function pointer for %s() in module %s is null.";
	NWOL_BEGIN_MODULE(::nwol, SDeviceDriverModule);
		NWOM_PFUNC	(::nwol, SDeviceDriverModule									, nwol_moduleTitle		);
		NWOM_PFUNC	(::nwol, SDeviceDriverModule									, nwol_moduleVersion	);
		NWOM_PFUNC	(::nwol, SDeviceDriverModule									, nwol_moduleCreate		);
		NWOM_PFUNC	(::nwol, SDeviceDriverModule									, nwol_moduleDelete		);
		NWOM_PFUNC	(::nwol, SDeviceDriverModule									, nwol_moduleSetup		);
		NWOM_PFUNC	(::nwol, SDeviceDriverModule									, nwol_moduleCleanup	);
		NWOM_PFUNC	(::nwol, SDeviceDriverModule									, nwol_moduleRender		);
		NWOM_PFUNC	(::nwol, SDeviceDriverModule									, nwol_moduleUpdate		);
		// -----------------------------------------------------------------------------------------------
		NWOM		(::nwol, SDeviceDriverModule, ::nwol	, SRuntimeValues	*	, RuntimeValues			, ::nwol::GDATA_TYPE_POINTER, "Runtime Values"	, "Holds global runtime information."									)	= nullptr;
		NWOM		(::nwol, SDeviceDriverModule, 			, void_t			*	, ClientInstance		, ::nwol::GDATA_TYPE_POINTER, "Client Instance"	, "Holds the client application instance created by moduleCreate()."	)	= nullptr;
		NWOM		(::nwol, SDeviceDriverModule, const		, char_t			*	, ModuleTitle			, ::nwol::GDATA_TYPE_POINTER, "Module Title"	, "The module title."													)	= nullptr;
		NWOM_REGISTRY
			(	NWOM_NAME(nwol_moduleTitle		)
			,	NWOM_NAME(nwol_moduleVersion	)
			,	NWOM_NAME(nwol_moduleCreate		)
			,	NWOM_NAME(nwol_moduleDelete		)
			,	NWOM_NAME(nwol_moduleSetup		)
			,	NWOM_NAME(nwol_moduleCleanup	)
			,	NWOM_NAME(nwol_moduleRender		)
			,	NWOM_NAME(nwol_moduleUpdate		)
			// ----------------------------------
			,	NWOM_NAME(RuntimeValues			)
			,	NWOM_NAME(ClientInstance		)
			,	NWOM_NAME(ModuleTitle			)
			);

		inline	uint8_t												VersionMajor					()										const	noexcept	{ const uint32_t version = Version(); return (uint8_t)((version & 0x00FF) >> 8); }
		inline	uint8_t												VersionMinor					()										const	noexcept	{ const uint32_t version = Version(); return (uint8_t)((version & 0x00FF) >> 0); }

				const char_t*										Title							()										const				{ retval_error_if(nullptr	,							0 == nwol_moduleTitle	, formatModuleFunctionPtrNull, "moduleTitle"	, ModuleTitle ? ModuleTitle : FilenameOriginal.c_str()); char		title [240]	= {}; uint8_t titleLen = (uint8_t)::nwol::size(title); nwol_moduleTitle(title, &titleLen); return ::nwol::glabel(title, titleLen).c_str();	}
		inline	uint32_t											Version							()										const				{ retval_error_if(0xFFFFFFFF,							0 == nwol_moduleVersion	, formatModuleFunctionPtrNull, "moduleVersion"	, ModuleTitle ? ModuleTitle : FilenameOriginal.c_str()); uint32_t moduleVer	=  0; retnul_error_if(errored(nwol_moduleVersion(&moduleVer)), "Failed to get module version."); return moduleVer;								}
		inline	::nwol::error_t										Create							()															{ retval_error_if(-1,									0 == nwol_moduleCreate	, formatModuleFunctionPtrNull, "moduleCreate"	, ModuleTitle ? ModuleTitle : FilenameOriginal.c_str()); return nwol_moduleCreate								(&ClientInstance, RuntimeValues);																			}
		inline	::nwol::error_t										Delete							()															{ retval_error_if(-1,									0 == nwol_moduleDelete	, formatModuleFunctionPtrNull, "moduleDelete"	, ModuleTitle ? ModuleTitle : FilenameOriginal.c_str()); return nwol_moduleDelete								(&ClientInstance);																							}
		inline	::nwol::error_t										Setup							()										const				{ retval_error_if(-1,									0 == nwol_moduleSetup	, formatModuleFunctionPtrNull, "moduleSetup"	, ModuleTitle ? ModuleTitle : FilenameOriginal.c_str()); return nwol_moduleSetup								(ClientInstance	);																							}
		inline	::nwol::error_t										Cleanup							()										const				{ retval_error_if(-1,									0 == nwol_moduleCleanup	, formatModuleFunctionPtrNull, "moduleCleanup"	, ModuleTitle ? ModuleTitle : FilenameOriginal.c_str()); return nwol_moduleCleanup								(ClientInstance	);																							}
		inline	::nwol::error_t										Render							()										const				{ retval_error_if(-1,									0 == nwol_moduleRender	, formatModuleFunctionPtrNull, "moduleRender"	, ModuleTitle ? ModuleTitle : FilenameOriginal.c_str()); return nwol_moduleRender								(ClientInstance	);																							}
		inline	::nwol::APPLICATION_STATE							Update							(bool bRequestedExit)					const				{ retval_error_if(::nwol::APPLICATION_STATE_INVALID	,	0 == nwol_moduleUpdate	, formatModuleFunctionPtrNull, "moduleUpdate"	, ModuleTitle ? ModuleTitle : FilenameOriginal.c_str()); return (::nwol::APPLICATION_STATE)nwol_moduleUpdate	(ClientInstance, bRequestedExit);																			}
				
				::nwol::RUNTIME_CALLBACK_ID							TestForNullPointerFunctions		()										const				{
			::nwol::RUNTIME_CALLBACK_ID										result							= RUNTIME_CALLBACK_ID_NONE;
			if(0 == nwol_moduleTitle		)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_TITLE		);
			if(0 == nwol_moduleVersion		)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_VERSION	);
			if(0 == nwol_moduleCreate		)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_CREATE		);
			if(0 == nwol_moduleDelete		)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_DELETE		);
			if(0 == nwol_moduleSetup		)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_SETUP		);
			if(0 == nwol_moduleCleanup		)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_CLEANUP	);
			if(0 == nwol_moduleRender		)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_RENDER		);
			if(0 == nwol_moduleUpdate		)	result						= (::nwol::RUNTIME_CALLBACK_ID)(result | ::nwol::RUNTIME_CALLBACK_ID_UPDATE		);
			return result;
		}
	NWOL_END_MODULE(::nwol, SDeviceDriverModule, "Defines a common interface for all the device driver modules.");

	 
	typedef			::nwol::SDeviceDriverModule					SApplicationModule;
	static inline	::nwol::error_t								applicationModuleUnload				(::nwol::SApplicationModule& containerForCallbacks)																		{ return ::nwol::moduleUnload(containerForCallbacks); }
					::nwol::error_t								applicationModuleLoad				(::nwol::SRuntimeValues& runtimeValues, ::nwol::SApplicationModule& containerForCallbacks, const char_t* moduleName);

					void										printErasedModuleInterfacePointers	(::nwol::RUNTIME_CALLBACK_ID erasedCallbacks, const char_t* errorFormat);
}	// namespace

namespace nwol
{
	struct SRuntimeValuesDetail {	
#if defined(__ANDROID__)
				ANativeActivity										* Activity							= nullptr;
				void												* SavedState						= nullptr;
				size_t												SavedStateSize						= 0;
#elif defined(__WINDOWS__)
				HINSTANCE											hInstance							= NULL;
				HINSTANCE											hPrevInstance						= NULL;
				LPSTR												lpCmdLine							= NULL;
				int													nShowCmd							= 0;

				// We need to add the window management in this abstraction level because in some systems we can't control the creation of the window and 
				// we require to mimic that situation from the beginning in order to keep platform compatibility.
				WNDCLASSEX											MainWindowClass						= {};
				DWORD												MainWindowStyle						= WS_OVERLAPPED | WS_THICKFRAME | WS_BORDER | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;
#else
#	error "Not implemented."
#endif
	};

	struct SRuntimeValues {	
				SRuntimeValuesDetail											PlatformDetail						= {};
				::nwol::view_const_string										CommandLine							= {};
				::nwol::array_obj<::nwol::view_const_string>					CommandLineArguments				= {};
				::nwol::view_const_string										FileNameApplication					= {};
				::nwol::view_const_string										FileNameRuntime						= {};
				::nwol::SScreen													Screen								= {};
				::nwol::array_obj<::nwol::ptr_nco<::nwol::SApplicationModule>>	Modules								= {};

				// Returns -1 on error or the index of the loaded module.
				::nwol::error_t													ModuleLoad							(const char_t* moduleName)	{
			for(uint32_t i=0; i<Modules.size(); ++i) 
				if(0 == strcmp(Modules[i]->FilenameOriginal.c_str(), moduleName)) 
					return i; 

			::nwol::ptr_obj<::nwol::SApplicationModule>									moduleInstance; 
			moduleInstance.create();
			nwol_necall(::nwol::applicationModuleLoad(*this, *moduleInstance, moduleName), "Module not loaded: %s.", moduleName); 
			return Modules.push_back(moduleInstance); 
		}
	};	// struct
}	// namespace

#endif // RUNTIME_H__29834908347

