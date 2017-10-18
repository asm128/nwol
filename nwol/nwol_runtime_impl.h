// These sources are best viewed in Visual Studio 2017 with a screen of at least 1920x1080 screen and the zoom set to 81 %.
// Pablo Ariel Zorrilla Cepeda (asm128) Copyright (c) 2010-2017 - Distributed under the MIT License.
#include "nwol_runtime.h"
#include "nwol_log.h"

#ifndef RUNTIME_IMPL_H_29834908347
#define RUNTIME_IMPL_H_29834908347

//--------------------------------------------------------------- Runtime Interface
#define DEFINE_RUNTIME_INTERFACE_TITLE(_yourCustomClass, _pluginTitle)																									\
::nwol::error_t	NWOL_STDCALL			::nwol_moduleTitle				(char_t* outputBuffer, uint8_t* outputBufferLength)					noexcept		{			\
	ree_if(0 == outputBufferLength, "Invalid parameters.");																												\
	static constexpr const char					customAppTitle[]				= _pluginTitle;																			\
	*outputBufferLength						= (uint8_t)(outputBuffer ? sprintf_s(outputBuffer, *outputBufferLength, customAppTitle) : strlen(customAppTitle));			\
	return 0; 																																							\
}

#define DEFINE_RUNTIME_INTERFACE_VERSION(_yourCustomClass, _versionMajor, _versionMinor)																				\
::nwol::error_t	NWOL_STDCALL			::nwol_moduleVersion			(uint32_t* version)													noexcept		{			\
	ree_if(0 == version, "Invalid parameter.");																															\
	static constexpr const uint16_t				_nwol_module_version			= ((_versionMinor & 0xFF) << 0) | ((_versionMajor & 0xFF) << 8);						\
	*version								= _nwol_module_version;																										\
	return 0; 																																							\
}

#define DEFINE_RUNTIME_INTERFACE_CREATE(_yourCustomClass)																												\
::nwol::error_t	NWOL_STDCALL			::nwol_moduleCreate				(void** instanceApp, ::nwol::SRuntimeValues* runtimeValues)			noexcept		{			\
	ree_if(instanceApp == 0 || 0 == runtimeValues, "Invalid parameters: instanceApp=0x%P, runtimeValues=0x%P", instanceApp, runtimeValues);								\
	_yourCustomClass							* newApp						= new _yourCustomClass(runtimeValues), *oldApp = (_yourCustomClass*)*instanceApp;		\
	ree_if(0 == newApp, "Cannot create application object. Not enough memory?");																						\
	*instanceApp							= newApp; 																													\
	safe_delete(oldApp); 																																				\
	return 0;																																							\
}

#define DEFINE_RUNTIME_INTERFACE_DELETE(_yourCustomClass)																												\
::nwol::error_t	NWOL_STDCALL			::nwol_moduleDelete				(void** instanceApp)												noexcept		{			\
	ree_if(instanceApp == 0, "%s", "Invalid pointer!");																													\
	_yourCustomClass							* oldApp						= (_yourCustomClass*)*instanceApp;														\
	*instanceApp							= 0; 																														\
	safe_delete(oldApp); 																																				\
	return 0;																																							\
}

#define DEFINE_RUNTIME_INTERFACE_SETUP(_yourCustomClass)	::nwol::error_t	NWOL_STDCALL	::nwol_moduleSetup		(void* instanceApp)		noexcept		{ ree_if(instanceApp == 0, "%s", "Invalid pointer!"); return setup		(*(_yourCustomClass*)instanceApp); }
#define DEFINE_RUNTIME_INTERFACE_CLEANUP(_yourCustomClass)	::nwol::error_t	NWOL_STDCALL	::nwol_moduleCleanup	(void* instanceApp)		noexcept		{ ree_if(instanceApp == 0, "%s", "Invalid pointer!"); return cleanup	(*(_yourCustomClass*)instanceApp); }
#define DEFINE_RUNTIME_INTERFACE_RENDER(_yourCustomClass)	::nwol::error_t	NWOL_STDCALL	::nwol_moduleRender		(void* instanceApp)		noexcept		{ ree_if(instanceApp == 0, "%s", "Invalid pointer!"); return render		(*(_yourCustomClass*)instanceApp); }
#define DEFINE_RUNTIME_INTERFACE_UPDATE(_yourCustomClass)																												\
::nwol::error_t	NWOL_STDCALL			::nwol_moduleUpdate				(void* instanceApp, bool requestedExit)								noexcept		{			\
	ree_if(instanceApp == 0, "%s", "Invalid pointer!");																													\
	_yourCustomClass							& customApp						= *(_yourCustomClass*)instanceApp;														\
	::nwol::APPLICATION_STATE					result							= (::nwol::APPLICATION_STATE)update(customApp, requestedExit);							\
	return result;																																						\
}

/// - In order to take advantage of the DEFINE_RUNTIME_INTERFACE_FUNCTIONS() macro below, your application will need to define an update(), 
/// setup(), render() and cleanup() functions taking a reference to your application instance as parameter as indicated in the following example snippet:
/// 
/// ---------- File: YourCustomAppHeader.h --------------------------
//#ifndef YOUR_CUSTOM_APP_HEADER_H_983264902
//#define YOUR_CUSTOM_APP_HEADER_H_983264902
//
/// Note that the moduleCreate() and moduleDelete functions require the full definition of your type as it has to create a full instance of the object.
// namespace YourNS {
//	struct YourCustomApp : public SFramework {
//		int					YourOptionalMember;
//							YourCustomApp			(SRuntimeValues* runtimeValues)		: SFramework(runtimeValues) {}	// <-- your custom constructor
//	}; 
// } // namespace
//int32_t setup		(YourNS::YourCustomApp& instanceApp);						// <--- These functions shall return 0 on success or -1 to indicate fatal failure and stop execution
//int32_t cleanup	(YourNS::YourCustomApp& instanceApp);						// <--- These functions shall return 0 on success or -1 to indicate fatal failure and stop execution
//int32_t render	(YourNS::YourCustomApp& instanceApp);						// <--- These functions shall return 0 on success or -1 to indicate fatal failure and stop execution
//int32_t update	(YourNS::YourCustomApp& instanceApp, bool requestedExit);	// <--- These functions shall return 0 on success or -1 to indicate fatal failure and stop execution
//
//#endif // YOUR_CUSTOM_APP_HEADER_H_983264902
// 
/// ---------- File: runtime_interface.cpp --------------------------
// #include "YourCustomAppHeader.h"	// <--- Your stuff from here
// #include "runtime_impl.h"		// <--- Runtime definition here
// 
// DEFINE_RUNTIME_INTERFACE_FUNCTIONS(YourCustomClass);	// <----- 
// 
/// ---------- File: YourCustomCodeOrWhateverYouCallIt.cpp ---------- 
// #include "YourCustomAppHeader.h"
// 
// int32_t setup	(YourNS::YourCustomApp& instanceApp);											{ return 0; } // This function will be called after the runtime instantiated your object with new().
// int32_t cleanup	(YourNS::YourCustomApp& instanceApp);											{ return 0; } // This function will be called after your app update() returns APPLICATION_STATE_EXIT and render() is not in course.
// int32_t render	(YourNS::YourCustomApp& instanceApp);											{ return 0; } // This function will be called in a loop, asynchronously with respect to update().
// int32_t update	(YourNS::YourCustomApp& instanceApp, bool requestedExit);						{ return 0; } // This function will be called in a loop, asynchronously with respect to render().

/// The DEFINE_RUNTIME_INTERFACE_FUNCTIONS() macro is used to reduce the bulk when creating a new shared library.
#define DEFINE_RUNTIME_INTERFACE_FUNCTIONS(_yourCustomClass, _pluginTitle, _versionMajor, _versionMinor)	\
	DEFINE_RUNTIME_INTERFACE_TITLE		(_yourCustomClass, _pluginTitle)									\
	DEFINE_RUNTIME_INTERFACE_VERSION	(_yourCustomClass, _versionMajor, _versionMinor)					\
	DEFINE_RUNTIME_INTERFACE_CREATE		(_yourCustomClass)													\
	DEFINE_RUNTIME_INTERFACE_DELETE		(_yourCustomClass)													\
	DEFINE_RUNTIME_INTERFACE_SETUP		(_yourCustomClass)													\
	DEFINE_RUNTIME_INTERFACE_CLEANUP	(_yourCustomClass)													\
	DEFINE_RUNTIME_INTERFACE_RENDER		(_yourCustomClass)													\
	DEFINE_RUNTIME_INTERFACE_UPDATE		(_yourCustomClass)

#endif // RUNTIME_IMPL_H_29834908347
