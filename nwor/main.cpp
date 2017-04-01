#include "runtime.h"
#include "multithread.h"

#if defined(__ANDROID__)
#include <android/native_activity.h>
#elif defined(__LINUX__)
#error "Not implemented."
#elif defined(__WINDOWS__)
#include <process.h>
#else 
#error "Platform not supported."
#endif

#include <thread>
#include <string>

struct SRuntimeState {	
#if defined(__WINDOWS__) || defined(__LINUX__) || defined(__ANDROID__)
	::nwol::refcount_t							RenderThreadUsers			= 0			;
#else
	::nwol::refcount_t							RenderThreadUsers			;
#endif
	::nwol::SModuleInterface					Interface					;
	bool										Quit						= false		;
	bool										Processing					= true		;
};

void										loopThreadRender				(SRuntimeState& stateRuntime)																				{
	debug_printf("Beginning render loop.");
	static const char								* errorFormat1					= "Dynamically loaded function is null, maybe due to a buffer overrun which erased the pointers: %s.";
	static const char								* errorFormat2					= "Module function failed with code 0x%x: %s.";
	::nwol::SModuleInterface						& containerForCallbacks			= stateRuntime.Interface;
	int32_t											lastError						= 0;
	while(false == stateRuntime.Quit) {
		int32_t											errRender = lastError			= containerForCallbacks.Render(); 
		if(0 > errRender) { 
			stateRuntime.Quit							= true; 
			error_printf(errorFormat2, errRender, "appRender()"); 
			break; 
		}

		::nwol::RUNTIME_CALLBACK_ID						callbackPointersErased			= containerForCallbacks.TestForNullPointerFunctions();
		if(callbackPointersErased) { 
			printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
			stateRuntime.Quit							= true; 
			break; 
		}
	}

	debug_printf("Render loop ended. Last error code: 0x%x.", (uint32_t)lastError);
}

void										runRender						(void* pStateRuntime)																						{
	if(0 == pStateRuntime) {
		error_printf("Runtime state pointer is null. Thread function exiting...");
		return;
	}

	debug_printf("Render thread started.");

	::SRuntimeState									& stateRuntime					= *(::SRuntimeState*)pStateRuntime;
	INTERLOCKED_INCREMENT	(stateRuntime.RenderThreadUsers);
	loopThreadRender		(stateRuntime);
	INTERLOCKED_DECREMENT	(stateRuntime.RenderThreadUsers);

	debug_printf("Render loop exited.");
}

int32_t										launchRenderThread				(::SRuntimeState& runtimeState)																				{
#if defined(__WINDOWS__)
	_beginthread(runRender, 0, &runtimeState);
#else
#error "Not implemented."
#endif
	return 0;
}

int32_t										mainLoop						(SRuntimeState & runtimeState, ::nwol::SModuleInterface& containerForCallbacks)								{
	typedef	uint8_t									RUNTIME_FLAG;
	static	const RUNTIME_FLAG						RUNTIME_FLAG_RUNNING			= 1;
	static	const RUNTIME_FLAG						RUNTIME_FLAG_NOT_YET_REQUESTED	= 2;

	RUNTIME_FLAG									executionState					= RUNTIME_FLAG_RUNNING | RUNTIME_FLAG_NOT_YET_REQUESTED;
	
	reterr_error_if_errored(launchRenderThread(runtimeState), "Failed to start render thread.");

	static	const char*								errorFormat2					= "Module function failed with code 0x%x: %s.";
	int32_t											errLoop							= 0;
	while(executionState & RUNTIME_FLAG_RUNNING) {
		::nwol::APPLICATION_STATE						errUpdate						= containerForCallbacks.Update(runtimeState.Quit); 
		switch(errUpdate) {
		case ::nwol::APPLICATION_STATE_NORMAL: 
			break;
		
		case ::nwol::APPLICATION_STATE_EXIT :
			runtimeState.Quit							= true;													
			runtimeState.Processing						= false;												
			break;											
	
		case ::nwol::APPLICATION_STATE_BUSY :
			runtimeState.Processing						= true;													
			break;											
		
		case ::nwol::APPLICATION_STATE_STAY :
			executionState								|= ~RUNTIME_FLAG_NOT_YET_REQUESTED;
			runtimeState.Quit							= false;
			runtimeState.Processing						= true;					
			break;
		
		default:
			error_printf(errorFormat2, errUpdate, "appUpdate()"); 
			errLoop										= -1;
			runtimeState.Quit							= true;													
			runtimeState.Processing						= false;												
			break;
		}
		
		::nwol::RUNTIME_CALLBACK_ID						callbackPointersErased			= containerForCallbacks.TestForNullPointerFunctions();
		static	const char*								errorFormat1					= "Dynamically loaded function is null, maybe due to a buffer overrun which erased the pointers: %s.";
		if(callbackPointersErased) { 
			printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
			errLoop										= -1;
			runtimeState.Quit							= true; 
			break; 
		}

		if(runtimeState.Quit) {
			if(false == runtimeState.Processing) {
				executionState								&= ~RUNTIME_FLAG_RUNNING;
				debug_printf("Execution shutting down...");
				break;
			}
			else if(executionState & RUNTIME_FLAG_NOT_YET_REQUESTED) {
				executionState								&= ~RUNTIME_FLAG_NOT_YET_REQUESTED;
				debug_printf("Client module requested exit.");
			}
		}
	}
	return errLoop;
}

int32_t										loadPlatformValues				(::nwol::SRuntimeValues& runtimeValues, const char* filenameApplication, char** args, uint32_t _argCount)	{
	runtimeValues.CommandLine					= args[0];

	int32_t											filenameStart					= (int32_t)strlen(runtimeValues.CommandLine)-1;

	do --filenameStart;
	while(filenameStart >= 0 && runtimeValues.CommandLine[filenameStart] != '\\' && runtimeValues.CommandLine[filenameStart] != '/');

	++filenameStart;	// we need to skip the bar character or fix the invalid index if no bar character found.

	runtimeValues.FileNameRuntime				= &runtimeValues.CommandLine[filenameStart];

	runtimeValues.CommandLineArgCount			= _argCount-1;
	runtimeValues.CommandLineArgCount			= (runtimeValues.CommandLineArgCount >= ::nwol::size(runtimeValues.CommandLineArgList)) ? ::nwol::size(runtimeValues.CommandLineArgList) : runtimeValues.CommandLineArgCount;
	for(uint32_t iArg = 0, argCount = runtimeValues.CommandLineArgCount; iArg<argCount; ++iArg)
		runtimeValues.CommandLineArgList[iArg]		= args[iArg+1];

	runtimeValues.FileNameApplication			= runtimeValues.CommandLineArgCount ? runtimeValues.CommandLineArgList[0] : filenameApplication;

	return 0;
};

int											rtMain							(::nwol::SRuntimeValues& runtimeValues)						
{
#if defined(__WINDOWS__) && defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF );	// Enable run-time memory check for debug builds.
	//_CrtSetBreakAlloc( 322 );
#endif	


	::nwol::SModuleInterface						preContainerForCallbacks		= {};
	preContainerForCallbacks.RuntimeValues			= &runtimeValues;
	::nwol::error_t									errMy							= ::nwol::loadModule(preContainerForCallbacks, runtimeValues.FileNameApplication);
	reterr_error_if_errored(errMy, "Failed to load module %s.", runtimeValues.FileNameApplication);
	
	::SRuntimeState									runtimeState					= {};
	::nwol::SModuleInterface						& containerForCallbacks			= runtimeState.Interface	= preContainerForCallbacks;

	static const char								* errorFormat1					= "Dynamically loaded function is null, maybe due to a buffer overrun which erased the pointers: %s.";
	static const char								* errorFormat2					= "Module function failed with code 0x%x: %s.";
	int32_t											errCreate						= containerForCallbacks.Create(); 
	if(0 > errCreate) { 
		runtimeState.Quit							= true; 
		error_printf(errorFormat2, errCreate, "appCreate()"); 
	} 
	else {
		::nwol::RUNTIME_CALLBACK_ID						callbackPointersErased			= containerForCallbacks.TestForNullPointerFunctions();
		if(callbackPointersErased) { 
			printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
			runtimeState.Quit							= true; 
		}
		else {
			int32_t											errSetup					= containerForCallbacks.Setup(); 
			if(0 > errSetup) { 
				runtimeState.Quit							= true; 
				error_printf(errorFormat2, errSetup, "appSetup()" ); 
			} 
			else {
				callbackPointersErased						= containerForCallbacks.TestForNullPointerFunctions();
				if(callbackPointersErased) { 
					printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
					runtimeState.Quit							= true; 
				}
				else {
					INTERLOCKED_INCREMENT(runtimeState.RenderThreadUsers);

					::nwol::error_t									errLoop						= mainLoop(runtimeState, containerForCallbacks);
					always_printf("Main loop exited with code %u.", (uint32_t)errLoop); 

					while(INTERLOCKED_COMPARE_EXCHANGE(runtimeState.RenderThreadUsers, 0, 1))
						::std::this_thread::sleep_for(::std::chrono::milliseconds(1));;

 					::nwol::error_t									errCleanup					= containerForCallbacks.Cleanup(); 
					if(0 > errCleanup) { 
						runtimeState.Quit							= true; 
						error_printf(errorFormat2, errCleanup, "appCleanup()"); 
					} 
					else {
						callbackPointersErased						= containerForCallbacks.TestForNullPointerFunctions();
						if(callbackPointersErased) { 
							printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
							runtimeState.Quit							= true; 
						}
					}
				}
			}
		}

		::nwol::error_t									errDelete						= containerForCallbacks.Delete(); 
		if(0 > errDelete) { 
			error_printf(errorFormat2, errDelete, "appDelete()"); 
		}

		callbackPointersErased						= containerForCallbacks.TestForNullPointerFunctions();
		if(callbackPointersErased) { 
			printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
			runtimeState.Quit							= true; 
		}
	}

	::nwol::unloadModule(containerForCallbacks);

	return 0;
}

#if defined(__ANDROID__)
// Android entry point. A note on app_dummy(): This is required as the compiler may otherwise remove the main entry point of the application
void										app_dummy						()																											{}
void										ANativeActivity_onCreate		(ANativeActivity* activity, void* savedState, size_t savedStateSize)										{
    debug_printf("Entering: %s", __FUNCTION__);
	app_dummy();	
	::nwol::SRuntimeValues							runtimeValues					= {};
	runtimeValues.PlatformValues.activity		= activity;
	runtimeValues.PlatformValues.savedState		= savedState;
	runtimeValues.PlatformValues.savedStateSize	= savedStateSize;

    //act->callbacks->onDestroy					= onDestroy					;
    //act->callbacks->onStart					= onStart					;
    //act->callbacks->onResume					= onResume					;
    //act->callbacks->onSaveInstanceState		= onSaveInstanceState		;
    //act->callbacks->onPause					= onPause					;
    //act->callbacks->onStop					= onStop					;
    //act->callbacks->onConfigurationChanged	= onConfigurationChanged	;
    //act->callbacks->onLowMemory				= onLowMemory				;
    //act->callbacks->onWindowFocusChanged		= onWindowFocusChanged		;
    //act->callbacks->onNativeWindowCreated		= onNativeWindowCreated		;
    //act->callbacks->onNativeWindowDestroyed	= onNativeWindowDestroyed	;
    //act->callbacks->onInputQueueCreated		= onInputQueueCreated		;
    //act->callbacks->onInputQueueDestroyed		= onInputQueueDestroyed		;

    activity->instance							= &runtimeValues;

	static const char								defaultModuleName[]				= "modules/nwor_selector.os";
	loadPlatformValues(runtimeValues, defaultModuleName, 0, 0);
	rtMain(runtimeValues);
    debug_printf("Exiting function normally: %s", __FUNCTION__);
}
#else
int											main							(int argc, char** argv)																						{ 
	::nwol::SRuntimeValues							runtimeValues					= {};
	static const char								defaultModuleName[]				= "modules/nwor_selector.os";
	loadPlatformValues(runtimeValues, defaultModuleName, argv, argc);
	rtMain(runtimeValues);
}
#	if defined (__WINDOWS__)
int WINAPI									WinMain 
	(    _In_		HINSTANCE	hInstance		
	,    _In_opt_	HINSTANCE	hPrevInstance	
	,    _In_		LPSTR		lpCmdLine		
	,    _In_		int			nShowCmd		
	)
{
	SetConsoleTitle("No Workflow Overhead Runtime");
	::nwol::SRuntimeValues							runtimeValues					= {};
	static const char								defaultModuleName[]				= "modules/nwor_selector.dll";
	runtimeValues.PlatformValues.hInstance		=  hInstance		;
	runtimeValues.PlatformValues.hPrevInstance	=  hPrevInstance	;
	runtimeValues.PlatformValues.lpCmdLine		=  lpCmdLine		;
	runtimeValues.PlatformValues.nShowCmd		=  nShowCmd			;

#if defined(__ANDROID__)
	loadPlatformValues(runtimeValues, defaultModuleName, 0, 0);
#else
	loadPlatformValues(runtimeValues, defaultModuleName, __argv, __argc);
#endif

	if(0 > rtMain(runtimeValues))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
#	endif
#endif