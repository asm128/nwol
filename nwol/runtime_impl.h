#include "runtime.h"
#include "log.h"

#ifndef __RUNTIME_IMPL_H__29834908347__
#define __RUNTIME_IMPL_H__29834908347__

//--------------------------------------------------------------- Runtime Interface
#define DEFINE_RUNTIME_INTERFACE_TITLE(_YourCustomClass, _PluginTitle)																						\
const char*								appTitle				(void)															{							\
	static constexpr const char						customAppTitle[]	= _PluginTitle;																		\
	return customAppTitle; 																																	\
}

#define DEFINE_RUNTIME_INTERFACE_VERSION(_YourCustomClass, _VersionMajor, _VersionMinor)																	\
uint8_t									appVersion				(void)															{							\
	static constexpr const uint8_t				version					= ((_VersionMajor & 0xF)<<4) | (_VersionMinor & 0xF);								\
	return version; 																																		\
}

#define DEFINE_RUNTIME_INTERFACE_CREATE(_YourCustomClass)																									\
int32_t									appCreate				(void** instanceApp, ::nwol::SRuntimeValues* runtimeValues)		{							\
	reterr_error_if(instanceApp == 0, "%s", "Invalid pointer!");																							\
	_YourCustomClass							* newApp				= new _YourCustomClass(runtimeValues), *oldApp = (_YourCustomClass*)*instanceApp; 	\
	*instanceApp							= newApp; 																										\
	safe_delete(oldApp); 																																	\
	return 0;																																				\
}

#define DEFINE_RUNTIME_INTERFACE_DELETE(_YourCustomClass)																									\
int32_t									appDelete				(void** instanceApp)											{							\
	reterr_error_if(instanceApp == 0, "%s", "Invalid pointer!");																							\
	_YourCustomClass							* oldApp				= (_YourCustomClass*)*instanceApp;													\
	*instanceApp							= 0; 																											\
	safe_delete(oldApp); 																																	\
	return 0;																																				\
}

#define DEFINE_RUNTIME_INTERFACE_SETUP(_YourCustomClass)	int32_t		appSetup	(void* instanceApp)							{ reterr_error_if(instanceApp == 0, "%s", "Invalid pointer!"); return setup		(*(_YourCustomClass*)instanceApp); }
#define DEFINE_RUNTIME_INTERFACE_CLEANUP(_YourCustomClass)	int32_t		appCleanup	(void* instanceApp)							{ reterr_error_if(instanceApp == 0, "%s", "Invalid pointer!"); return cleanup	(*(_YourCustomClass*)instanceApp); }
#define DEFINE_RUNTIME_INTERFACE_RENDER(_YourCustomClass)	int32_t		appRender	(void* instanceApp)							{ reterr_error_if(instanceApp == 0, "%s", "Invalid pointer!"); return render	(*(_YourCustomClass*)instanceApp); }

#define DEFINE_RUNTIME_INTERFACE_UPDATE(_YourCustomClass)																									\
::nwol::APPLICATION_STATE				appUpdate				(void* instanceApp, bool requestedExit)							{							\
	if(instanceApp == 0) {																																	\
		error_printf("%s", "Invalid pointer!");																												\
		return ::nwol::APPLICATION_STATE_FATAL;																												\
	}																																						\
																																							\
	_YourCustomClass							& customApp				= *(_YourCustomClass*)instanceApp;													\
	::nwol::APPLICATION_STATE					result					= (::nwol::APPLICATION_STATE)update(customApp, requestedExit);						\
	return result;																																			\
}


///
/// - In order to take advantage of the DEFINE_RUNTIME_INTERFACE_FUNCTIONS() macro below, your application will need to define an update(), 
/// setup(), render() and cleanup() functions taking a reference to your application instance as parameter
/// as indicated in the following example snippet:
/// 
/// ---------- File: YourCustomAppHeader.h --------------------------
//
//#ifndef __YOURCUSTOMAPPHEADER_H__983264902__
//#define __YOURCUSTOMAPPHEADER_H__983264902__
//
/// Note that the appCreate() function requires the full definition of your type as it has to create a full instance of the object.
// namespace YourNS {
//	struct YourCustomApp  {
//		int YourOptionalMember;
//	}; 
// } // namespace
//int32_t setup		(YourNS::YourCustomApp& instanceApp);	// <--- These functions shall return 0 on success or -1 to indicate fatal failure and stop execution
//int32_t cleanup	(YourNS::YourCustomApp& instanceApp);	// <--- These functions shall return 0 on success or -1 to indicate fatal failure and stop execution
//int32_t render	(YourNS::YourCustomApp& instanceApp);	// <--- These functions shall return 0 on success or -1 to indicate fatal failure and stop execution
//int32_t update	(YourNS::YourCustomApp& instanceApp);	// <--- These functions shall return 0 on success or -1 to indicate fatal failure and stop execution
//
//#endif // __YOURCUSTOMAPPHEADER_H__983264902__
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
#define DEFINE_RUNTIME_INTERFACE_FUNCTIONS(_YourCustomClass, _PluginTitle, _VersionMajor, _VersionMinor)	\
	DEFINE_RUNTIME_INTERFACE_TITLE		(_YourCustomClass, _PluginTitle)									\
	DEFINE_RUNTIME_INTERFACE_VERSION	(_YourCustomClass, _VersionMajor, _VersionMinor)					\
	DEFINE_RUNTIME_INTERFACE_CREATE		(_YourCustomClass)													\
	DEFINE_RUNTIME_INTERFACE_DELETE		(_YourCustomClass)													\
	DEFINE_RUNTIME_INTERFACE_SETUP		(_YourCustomClass)													\
	DEFINE_RUNTIME_INTERFACE_CLEANUP	(_YourCustomClass)													\
	DEFINE_RUNTIME_INTERFACE_RENDER		(_YourCustomClass)													\
	DEFINE_RUNTIME_INTERFACE_UPDATE		(_YourCustomClass)

#endif // __RUNTIME_IMPL_H__29834908347__
