#include "nwor.h"

extern	::nwor::SRuntimeState								* g_RuntimeState;

static	int32_t												loadPlatformValues				(::nwol::SRuntimeValues& runtimeValues, const char* filenameApplication, char** args, uint32_t _argCount)	{
	if(args) {
		runtimeValues.CommandLine									= args[0];

		int32_t															filenameStart					= (int32_t)strlen(runtimeValues.CommandLine)-1;

		do --filenameStart;
		while(filenameStart >= 0 && runtimeValues.CommandLine[filenameStart] != '\\' && runtimeValues.CommandLine[filenameStart] != '/');

		++filenameStart;	// we need to skip the bar character or fix the invalid index if no bar character found.

		runtimeValues.FileNameRuntime								= &runtimeValues.CommandLine[filenameStart];
		runtimeValues.CommandLineArgCount							= _argCount-1;
		runtimeValues.CommandLineArgCount							= (runtimeValues.CommandLineArgCount >= ::nwol::size(runtimeValues.CommandLineArgList)) ? ::nwol::size(runtimeValues.CommandLineArgList) : runtimeValues.CommandLineArgCount;
		for(uint32_t iArg = 0, argCount = runtimeValues.CommandLineArgCount; iArg<argCount; ++iArg)
			runtimeValues.CommandLineArgList[iArg]						= args[iArg+1];
	} 
	else {
		runtimeValues.FileNameRuntime								= 0;
		runtimeValues.CommandLineArgCount							= 0;
		runtimeValues.CommandLine									= 0;
		memset(runtimeValues.CommandLineArgList, 0, sizeof(const char_t*)*::nwol::size(runtimeValues.CommandLineArgList));
	}

	runtimeValues.FileNameApplication							= runtimeValues.CommandLineArgCount ? runtimeValues.CommandLineArgList[0] : filenameApplication;
	return 0;
}

#if defined(__ANDROID__)
// Android entry point. A note on app_dummy(): This is required as the compiler may otherwise remove the main entry point of the application
		void												app_dummy						()																											{}
		void												ANativeActivity_onCreate		(ANativeActivity* activity, void* savedState, size_t savedStateSize)										{
    info_printf("Entering: %s", __FUNCTION__);
	app_dummy();	

	::nwor::SRuntimeState											runtimeState					= {};	
	::nwol::SRuntimeValues											& runtimeValues					= runtimeState.RuntimeValues;

	runtimeValues.PlatformDetail.activity						= activity;
	runtimeValues.PlatformDetail.savedState						= savedState;
	runtimeValues.PlatformDetail.savedStateSize					= savedStateSize;
	activity->instance											= &runtimeValues;

	g_RuntimeState												= &runtimeState;

	static const char_t												defaultModuleName[]				= "modules/nwor_selector." DYNAMIC_LIBRARY_EXTENSION;
	::nwol::error_t													errMy							= 0;		
		 error_if(errored(errMy = ::loadPlatformValues(runtimeValues, defaultModuleName, 0, 0)	), "%s() failed with code: 0x%X", "loadPlatformValues", errMy)
	else error_if(errored(errMy = ::nwor::rtMain(runtimeState)									), "%s() failed with code: 0x%X", "rtMain");  
    info_printf("Exiting function normally: %s", __FUNCTION__);
}
#else
		int													main							(int argc, char** argv)																						{ 
	::nwor::SRuntimeState											runtimeState					= {};	
	::nwol::SRuntimeValues											& runtimeValues					= runtimeState.RuntimeValues;

	g_RuntimeState												= &runtimeState;

	static const char_t												defaultModuleName[]				= "modules/nwor_selector." DYNAMIC_LIBRARY_EXTENSION;
	if( 0 > loadPlatformValues(runtimeValues, defaultModuleName, argv, argc) )
		return -1;

	::nwol::error_t													errMy							= 0;
	retval_error_if(EXIT_FAILURE, errored(errMy = ::loadPlatformValues(runtimeValues, defaultModuleName, __argv, __argc)	), "%s() failed.", "loadPlatformValues"	);  
	retval_error_if(EXIT_FAILURE, errored(errMy = ::nwor::rtMain(runtimeState)												), "%s() failed.", "rtMain"				);  
	return 0;
}
#	if defined (__WINDOWS__)
		int WINAPI											WinMain 
	(    _In_		HINSTANCE	hInstance		
	,    _In_opt_	HINSTANCE	hPrevInstance	
	,    _In_		LPSTR		lpCmdLine		
	,    _In_		int			nShowCmd		
	)
{
	SetConsoleTitle("No Workflow Overhead Runtime");
	::nwor::SRuntimeState											runtimeState					= {};	
	::nwol::SRuntimeValues											& runtimeValues					= runtimeState.RuntimeValues;
	runtimeValues.PlatformDetail.hInstance						=  hInstance		;
	runtimeValues.PlatformDetail.hPrevInstance					=  hPrevInstance	;
	runtimeValues.PlatformDetail.lpCmdLine						=  lpCmdLine		;
	runtimeValues.PlatformDetail.nShowCmd						=  nShowCmd			;

	g_RuntimeState												= &runtimeState;

	static const char_t												defaultModuleName[]				= "modules/nwor_selector." DYNAMIC_LIBRARY_EXTENSION;
	::nwol::error_t													errMy							= 0;
	retval_error_if(EXIT_FAILURE, errored(errMy = ::loadPlatformValues(runtimeValues, defaultModuleName, __argv, __argc)	), "%s() failed.", "loadPlatformValues"	);  
	retval_error_if(EXIT_FAILURE, errored(errMy = ::nwor::rtMain(runtimeState)												), "%s() failed.", "rtMain"				);  
	return EXIT_SUCCESS;
}
#	endif
#endif
