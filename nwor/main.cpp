#include "nwor.h"

extern	::nwor::SRuntimeState								* g_RuntimeState;

static	int32_t												loadPlatformValues				(::nwol::SRuntimeValues& runtimeValues, const char* filenameApplication, char** args, uint32_t _argCount)	{
	if(args) {
		runtimeValues.CommandLine									= {args[0], (uint32_t)strlen(args[0])};

		int32_t															filenameStart					= (int32_t)runtimeValues.CommandLine.size() - 1;

		do --filenameStart;
		while(filenameStart >= 0 && runtimeValues.CommandLine[filenameStart] != '\\' && runtimeValues.CommandLine[filenameStart] != '/');

		++filenameStart;	// we need to skip the bar character or fix the invalid index if no bar character found.

		runtimeValues.FileNameRuntime								= {&runtimeValues.CommandLine[filenameStart], (uint32_t)strlen(&runtimeValues.CommandLine[filenameStart])};
		runtimeValues.CommandLineArguments.resize(_argCount);
		for(uint32_t iArg = 0; iArg < _argCount; ++iArg) 
			runtimeValues.CommandLineArguments[iArg]					= {args[iArg], (uint32_t)strlen(args[iArg])};
	} 
	else {
		runtimeValues.FileNameRuntime								= {};
		runtimeValues.CommandLine									= {};
		runtimeValues.CommandLineArguments							= {};
	}

	runtimeValues.FileNameApplication							
		= runtimeValues.CommandLineArguments.size() > 1 ? runtimeValues.CommandLineArguments[1]
		: runtimeValues.CommandLineArguments.size() > 0 ? runtimeValues.CommandLineArguments[0]
		: ::nwol::view_const_string{filenameApplication, (uint32_t)strlen(filenameApplication)}
		;
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
	else error_if(errored(errMy = ::nwor::rtMain(runtimeState)									), "%s() failed with code: 0x%X", "rtMain")
    else info_printf("Exiting function normally: %s", __FUNCTION__);
}
#else
		int													main							(int argc, char** argv)																						{ 
	::nwor::SRuntimeState											runtimeState					= {};	
	::nwol::SRuntimeValues											& runtimeValues					= runtimeState.RuntimeValues;

	g_RuntimeState												= &runtimeState;

	static const char_t												defaultModuleName[]				= "modules/nwor_selector." DYNAMIC_LIBRARY_EXTENSION;
	::nwol::error_t													errMy							= 0;
	rve_if(EXIT_FAILURE, errored(errMy = ::loadPlatformValues(runtimeValues, defaultModuleName, argv, argc)	), "%Error code: 0x%X.", errMy	);  
	rve_if(EXIT_FAILURE, errored(errMy = ::nwor::rtMain(runtimeState)										), "%Error code: 0x%X.", errMy	);  
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
	::nwol::SWindowsEntryPointArgs									& entryPointArgs				= runtimeValues.PlatformDetail.EntryPointArgs;
	entryPointArgs.hInstance									= hInstance		;
	entryPointArgs.hPrevInstance								= hPrevInstance	;
	entryPointArgs.lpCmdLine									= lpCmdLine		;
	entryPointArgs.nShowCmd										= nShowCmd		;

	g_RuntimeState												= &runtimeState;

	static const char_t												defaultModuleName[]				= "modules/nwor_selector." DYNAMIC_LIBRARY_EXTENSION;
	::nwol::error_t													errMy							= 0;
	retval_error_if(EXIT_FAILURE, errored(errMy = ::loadPlatformValues(runtimeValues, defaultModuleName, __argv, __argc)	), "%Error code: 0x%X.", errMy	);  
	retval_error_if(EXIT_FAILURE, errored(errMy = ::nwor::rtMain(runtimeState)												), "%Error code: 0x%X.", errMy	);  
	return EXIT_SUCCESS;
}
#	endif
#endif
