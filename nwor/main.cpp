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
	::nwol::refcount_t											RenderThreadUsers						= 0			;
#else
	::nwol::refcount_t											RenderThreadUsers						;
#endif
	::nwol::SModuleInterface									Interface								= {}		;
	::nwol::SRuntimeValues										RuntimeValues							= {}		;
	bool														Quit									= false		;
	bool														Processing								= true		;
};

::SRuntimeState												* g_RuntimeState						= nullptr;

void														renderLoop						(SRuntimeState& stateRuntime)																	{
	info_printf("Beginning render loop.");
	static const char												* errorFormat1							= "Dynamically loaded function is null, maybe due to a buffer overrun which erased the pointers: %s.";
	static const char												* errorFormat2							= "Module function failed with code 0x%x: %s.";
	::nwol::SModuleInterface										& containerForCallbacks					= stateRuntime.Interface;
	int32_t															lastError								= 0;
	while(false == stateRuntime.Quit) {
		int32_t															errRender = lastError					= containerForCallbacks.Render(); 
		if(0 > errRender) { 
			stateRuntime.Quit											= true; 
			error_printf(errorFormat2, errRender, "moduleRender()"); 
			break; 
		}

		::nwol::RUNTIME_CALLBACK_ID										callbackPointersErased					= containerForCallbacks.TestForNullPointerFunctions();
		if(callbackPointersErased) { 
			printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
			stateRuntime.Quit											= true; 
			break; 
		}
	}

	info_printf("Render loop ended. Last error code: 0x%x.", (uint32_t)lastError);
}

void														renderThread							(void* pStateRuntime)																						{
	if(0 == pStateRuntime) {
		error_printf("Runtime state pointer is null. Thread function exiting...");
		return;
	}

	info_printf("Render thread started.");

	::SRuntimeState													& stateRuntime							= *(::SRuntimeState*)pStateRuntime;
	INTERLOCKED_INCREMENT	(stateRuntime.RenderThreadUsers);
	renderLoop				(stateRuntime);
	INTERLOCKED_DECREMENT	(stateRuntime.RenderThreadUsers);

	info_printf("Render loop exited.");
}

int32_t														launchRenderThread						(::SRuntimeState& runtimeState)																				{
#if defined(__WINDOWS__)
	_beginthread(renderThread, 0, &runtimeState);
#else
#error "Not implemented."
#endif
	return 0;
}

LRESULT	WINAPI												mainWndProc								(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


int32_t														mainLoop								(SRuntimeState & runtimeState, ::nwol::SModuleInterface& containerForCallbacks)								{
	typedef				uint8_t										RUNTIME_FLAG;
	static constexpr	const RUNTIME_FLAG							RUNTIME_FLAG_RUNNING					= 1;
	static constexpr	const RUNTIME_FLAG							RUNTIME_FLAG_NOT_YET_REQUESTED			= 2;

	RUNTIME_FLAG													executionState							= RUNTIME_FLAG_RUNNING | RUNTIME_FLAG_NOT_YET_REQUESTED;
	
	reterr_error_if_errored(launchRenderThread(runtimeState), "Failed to start render thread.");

	static constexpr	const char										errorFormat2[]						= "Module function failed with code 0x%x: %s.";
	int32_t															errLoop									= 0;
	MSG																windowMessage							= {};
	HWND															mainWindow								= runtimeState.RuntimeValues.Screen.PlatformDetail.hWnd;
	while(executionState & RUNTIME_FLAG_RUNNING) {
		while(PeekMessageA(&windowMessage, mainWindow, 0, 0, PM_REMOVE)) {
			TranslateMessage( &windowMessage );
			DispatchMessage	( &windowMessage );
		}
		while(PeekMessageA(&windowMessage, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage( &windowMessage );
			DispatchMessage	( &windowMessage );
		}
		::nwol::APPLICATION_STATE										errUpdate								= containerForCallbacks.Update(runtimeState.Quit || runtimeState.RuntimeValues.Screen.State.Closed); 
		runtimeState.RuntimeValues.Screen.State.Closed				= false;
		switch(errUpdate) {
		case ::nwol::APPLICATION_STATE_NORMAL: 
			break;
		
		case ::nwol::APPLICATION_STATE_EXIT :
			runtimeState.Quit											= true;													
			runtimeState.Processing										= false;												
			break;															
	
		case ::nwol::APPLICATION_STATE_BUSY :
			runtimeState.Processing										= true;													
			break;															
		
		case ::nwol::APPLICATION_STATE_STAY :
			executionState												|= ~RUNTIME_FLAG_NOT_YET_REQUESTED;
			runtimeState.Quit											= false;
			runtimeState.Processing										= true;					
			break;
		
		default:
			error_printf(errorFormat2, errUpdate, "moduleUpdate()"); 
			errLoop														= -1;
			runtimeState.Quit											= true;													
			runtimeState.Processing										= false;												
			break;
		}
		
		::nwol::RUNTIME_CALLBACK_ID										callbackPointersErased					= containerForCallbacks.TestForNullPointerFunctions();
		static constexpr	const char									errorFormat1[]							= "Dynamically loaded function is null, maybe due to a buffer overrun which erased the pointers: %s.";
		if(callbackPointersErased) { 
			printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
			errLoop														= -1;
			runtimeState.Quit											= true; 
			break; 
		}

		if(runtimeState.Quit) {
			if(false == runtimeState.Processing) {
				executionState												&= ~RUNTIME_FLAG_RUNNING;
				info_printf("Execution shutting down...");
				break;
			}
			else if(executionState & RUNTIME_FLAG_NOT_YET_REQUESTED) {
				executionState												&= ~RUNTIME_FLAG_NOT_YET_REQUESTED;
				info_printf("Client module requested exit.");
			}
		}
	}
	return errLoop;
}

LRESULT	WINAPI												mainWndProc								(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)	{
	::nwol::SScreenState											& screenState							= g_RuntimeState->RuntimeValues.Screen.State;
	switch( uMsg ) {
	case WM_DESTROY:
		info_printf( "Received WM_DESTROY message." );
		return 0;
	case WM_QUIT:
		info_printf( "Received WM_QUIT message. return DefWindowProc( hWnd=%p, uMsg=%i, wParam=%i, lParam=%i )...", hWnd, (int)uMsg, (int)wParam, (int)lParam );
		break;
	case WM_SIZE:
		if( wParam == SIZE_MINIMIZED ) {
			info_printf("Received SIZE_MINIMIZED.");
			screenState.MinOrMaxed = screenState.NoDraw					= true;
			break;
		}
		else if( wParam == SIZE_MAXIMIZED ) {
			info_printf("Received SIZE_MAXIMIZED.");
			screenState.RequiresResizeWindow = screenState.MinOrMaxed	= true;
			screenState.NoDraw											= false;
		}
		else if( wParam == SIZE_RESTORED ) {
			info_printf("Received SIZE_RESTORED.");
			screenState.RequiresResizeWindow = screenState.MinOrMaxed	= true;
			screenState.NoDraw											= false;
		}
		else {
			info_printf("Received unknown WM_SIZE message.");
			//State.RequiresResizeWindow						= true;
			screenState.MinOrMaxed = screenState.NoDraw					= false;
		}

		break;
	case WM_SHOWWINDOW:
	case WM_CREATE:
	case WM_EXITSIZEMOVE:
		info_printf("Received WM_EXITSIZEMOVE message.");
		screenState.NoDraw											= false;
		screenState.RequiresResizeWindow							= true;
		break;
	case WM_MENUCHAR:	// The WM_MENUCHAR message is sent when a menu is active and the user presses a key that does not correspond to any mnemonic or accelerator key. 
		return MAKELRESULT(0, MNC_CLOSE);	// Don't beep when we alt-enter.
		break;
	case WM_GETMINMAXINFO:	// Catch this message so to prevent the window from becoming too small.
		((MINMAXINFO*)lParam)->ptMinTrackSize.x						= 320;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y						= 200; 
		return 0;
	case WM_SYSKEYDOWN:
		return 0;
	case WM_CLOSE:
		info_printf( "Received WM_CLOSE message." );
		screenState.Closed											= true;
		return 0;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

int32_t														setupScreen								(::nwol::SRuntimeValues& runtimeValues, const char_t* windowTitle)							{ 
	WNDCLASSEX														windowClass								= {};
	windowClass.cbSize											= sizeof(WNDCLASSEX);
	windowClass.style											= NULL; // CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc										= mainWndProc; 
	windowClass.cbClsExtra										= 0;
	windowClass.cbWndExtra										= 0;
	windowClass.hInstance										= runtimeValues.PlatformDetail.hInstance;
	windowClass.hIcon											= LoadIcon(0, IDI_APPLICATION);
	windowClass.hCursor											= LoadCursor(0, IDC_ARROW);
	windowClass.hbrBackground									= CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	windowClass.lpszMenuName									= 0;
	windowClass.lpszClassName									= "nwor_screen";
	windowClass.hIconSm											= LoadIcon(0, IDI_WINLOGO);
	DWORD															dwStyle									= WS_OVERLAPPED | WS_THICKFRAME | WS_BORDER | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;
	runtimeValues.PlatformDetail.MainWindowClass				= windowClass;

	bool_t															bClassRegistered						= (RegisterClassExA(&runtimeValues.PlatformDetail.MainWindowClass) != 0) ? true : false;
	reterr_error_if(!bClassRegistered, "Failed to register WNDCLASS \"%s\".", windowClass.lpszClassName);

	HWND															newWindow								= CreateWindowExA
		(	0L
		,	"nwor_screen"
		,	windowTitle
		,	dwStyle
		,	runtimeValues.Screen.Metrics.Position.x
		,	runtimeValues.Screen.Metrics.Position.y
		,	runtimeValues.Screen.Metrics.Size.x + GetSystemMetrics( SM_CXFRAME ) * 2
		,	runtimeValues.Screen.Metrics.Size.y + GetSystemMetrics( SM_CYFRAME ) * 2 + GetSystemMetrics( SM_CYCAPTION )
		,	0, 0, windowClass.hInstance, 0
		); 
	reterr_error_if(0 == newWindow, "CreateWindow FAILED!");

	runtimeValues.Screen.PlatformDetail	.hWnd					= newWindow;
	runtimeValues.PlatformDetail		.MainWindowStyle		= dwStyle;
	ShowWindow(newWindow, SW_SHOW);
	return 0;
}

int32_t														shutdownScreen							(::SRuntimeState& instanceApp)							{ 
	::nwol::SScreenState											& screenState							= instanceApp.RuntimeValues.Screen.State;
	::nwol::SScreenDetail											& screenDetail							= instanceApp.RuntimeValues.Screen.PlatformDetail;
	if( screenDetail.hWnd ) {
		HWND															oldWindow								= screenDetail.hWnd;
		screenState.NoDraw											= true;
		screenDetail.hWnd											= 0;
		MSG																windowMessage;
		info_printf( "Calling DestroyWindow()." );
		DestroyWindow( oldWindow );
		while(PeekMessageA(&windowMessage, oldWindow, 0, 0, PM_REMOVE)) {
			TranslateMessage( &windowMessage );
			DispatchMessageA( &windowMessage );
		}
		bool_t															bClassUnregistered						= (UnregisterClass(instanceApp.RuntimeValues.PlatformDetail.MainWindowClass.lpszClassName, instanceApp.RuntimeValues.PlatformDetail.MainWindowClass.hInstance) != 0) ? true : false;
		std::string														windowsError							= ::nwol::getWindowsErrorAsString(GetLastError());
		reterr_error_if(!bClassUnregistered, "Failed to unregister WNDCLASS \"%s\". \"%s\"", instanceApp.RuntimeValues.PlatformDetail.MainWindowClass.lpszClassName, windowsError.c_str());
	}

	return 0;
}

int															rtMain									(::SRuntimeState& runtimeState)	{
#if defined(__WINDOWS__) && defined(NWOL_DEBUG_ENABLED)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF );	// Enable run-time memory check for debug builds.
	//_CrtSetBreakAlloc( 322 );
#endif	

	::nwol::SModuleInterface										preContainerForCallbacks				= {};
	preContainerForCallbacks.RuntimeValues						= &runtimeState.RuntimeValues;
	::nwol::error_t													errMy									= ::nwol::loadModule(preContainerForCallbacks, runtimeState.RuntimeValues.FileNameApplication);
	reterr_error_if_errored(errMy, "Failed to load module %s.", runtimeState.RuntimeValues.FileNameApplication);
	
	::nwol::SModuleInterface										& containerForCallbacks					= runtimeState.Interface	= preContainerForCallbacks;
	char															windowTitle[512]						= {};
	sprintf_s(windowTitle, "%s v%u.%u", containerForCallbacks.ModuleTitle, containerForCallbacks.VersionMajor(), containerForCallbacks.VersionMinor());

	reterr_error_if(0 > ::setupScreen(runtimeState.RuntimeValues, windowTitle), "Failed to create main window.");

	static const char												* errorFormat1							= "Dynamically loaded function is null, maybe due to a buffer overrun which erased the pointers: %s.";
	static const char												* errorFormat2							= "Module function failed with code 0x%x: %s.";
	int32_t															errCreate								= containerForCallbacks.Create(); 
	if(0 > errCreate) { 
		runtimeState.Quit											= true; 
		error_printf(errorFormat2, errCreate, "moduleCreate()"); 
	} 
	else {
		::nwol::RUNTIME_CALLBACK_ID										callbackPointersErased					= containerForCallbacks.TestForNullPointerFunctions();
		if(callbackPointersErased) { 
			printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
			runtimeState.Quit											= true; 
		}
		else {
			int32_t															errSetup								= containerForCallbacks.Setup(); 
			if(0 > errSetup) { 
				runtimeState.Quit											= true; 
				error_printf(errorFormat2, errSetup, "moduleSetup()" ); 
			} 
			else {
				callbackPointersErased										= containerForCallbacks.TestForNullPointerFunctions();
				if(callbackPointersErased) { 
					printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
					runtimeState.Quit											= true; 
				}
				else {
					INTERLOCKED_INCREMENT(runtimeState.RenderThreadUsers);

					::nwol::error_t													errLoop									= mainLoop(runtimeState, containerForCallbacks);
					always_printf("Main loop exited with code %u.", (uint32_t)errLoop); 

					while(INTERLOCKED_COMPARE_EXCHANGE(runtimeState.RenderThreadUsers, 0, 1))
						::std::this_thread::sleep_for(::std::chrono::milliseconds(1));;
						
 					::nwol::error_t													errCleanup								= containerForCallbacks.Cleanup(); 
					if(0 > errCleanup) { 
						runtimeState.Quit											= true; 
						error_printf(errorFormat2, errCleanup, "moduleCleanup()"); 
					} 
					else {
						callbackPointersErased										= containerForCallbacks.TestForNullPointerFunctions();
						if(callbackPointersErased) { 
							printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
							runtimeState.Quit											= true; 
						}
					}
				}
			}
		}

#if defined(__WINDOWS__)
		PostQuitMessage(0);
		MSG																windowMessage							= {};
		while(WM_QUIT != windowMessage.message) 
			while(PeekMessageA(&windowMessage, 0, 0, 0, PM_REMOVE)) {
				TranslateMessage( &windowMessage );
				DispatchMessage	( &windowMessage );
			}
#endif

		::nwol::error_t													errDelete								= containerForCallbacks.Delete(); 
		if(0 > errDelete) { 
			error_printf(errorFormat2, errDelete, "moduleDelete()"); 
		}

		callbackPointersErased										= containerForCallbacks.TestForNullPointerFunctions();
		if(callbackPointersErased) { 
			printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
			runtimeState.Quit											= true; 
		}
	}



	::nwol::unloadModule(containerForCallbacks);

	::shutdownScreen(runtimeState);

	return 0;
}

int32_t														loadPlatformValues				(::nwol::SRuntimeValues& runtimeValues, const char* filenameApplication, char** args, uint32_t _argCount)	{
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
void														app_dummy						()																											{}
void														ANativeActivity_onCreate		(ANativeActivity* activity, void* savedState, size_t savedStateSize)										{
    info_printf("Entering: %s", __FUNCTION__);
	app_dummy();	

	::SRuntimeState													runtimeState					= {};	
	::nwol::SRuntimeValues											& runtimeValues					= runtimeState.RuntimeValues;

	runtimeValues.PlatformDetail.activity						= activity;
	runtimeValues.PlatformDetail.savedState						= savedState;
	runtimeValues.PlatformDetail.savedStateSize					= savedStateSize;
	activity->instance											= &runtimeValues;

	g_RuntimeState												= &runtimeState;

	static const char_t												defaultModuleName[]				= "modules/nwor_selector." DYNAMIC_LIBRARY_EXTENSION;
	::nwol::error_t											
	errMy														= ::loadPlatformValues(runtimeValues, defaultModuleName, 0, 0);		
	error_if(errored(errMy), "%s() failed with code: 0x%X", "loadPlatformValues", errMy)
	else {
		errMy														= ::rtMain(runtimeState);												
		error_if(errored(errMy), "%s() failed with code: 0x%X", "rtMain"				);  
	}
    info_printf("Exiting function normally: %s", __FUNCTION__);
}
#else
int															main							(int argc, char** argv)																						{ 
	::SRuntimeState													runtimeState					= {};	
	::nwol::SRuntimeValues											& runtimeValues					= runtimeState.RuntimeValues;

	g_RuntimeState												= &runtimeState;

	static const char_t												defaultModuleName[]				= "modules/nwor_selector." DYNAMIC_LIBRARY_EXTENSION;
	if( 0 > loadPlatformValues(runtimeValues, defaultModuleName, argv, argc) )
		return -1;

	::nwol::error_t											
	errMy														= ::loadPlatformValues(runtimeValues, defaultModuleName, __argv, __argc);		retval_error_if_errored(1, errMy, "%s() failed.", "loadPlatformValues"	);  
	errMy														= ::rtMain(runtimeState);														retval_error_if_errored(1, errMy, "%s() failed.", "rtMain"				);  
	return 0;
}
#	if defined (__WINDOWS__)
int WINAPI													WinMain 
	(    _In_		HINSTANCE	hInstance		
	,    _In_opt_	HINSTANCE	hPrevInstance	
	,    _In_		LPSTR		lpCmdLine		
	,    _In_		int			nShowCmd		
	)
{
	SetConsoleTitle("No Workflow Overhead Runtime");
	::SRuntimeState													runtimeState					= {};	

	::nwol::SRuntimeValues											& runtimeValues					= runtimeState.RuntimeValues;
	runtimeValues.PlatformDetail.hInstance						=  hInstance		;
	runtimeValues.PlatformDetail.hPrevInstance					=  hPrevInstance	;
	runtimeValues.PlatformDetail.lpCmdLine						=  lpCmdLine		;
	runtimeValues.PlatformDetail.nShowCmd						=  nShowCmd			;

	g_RuntimeState												= &runtimeState;

	static const char_t												defaultModuleName[]				= "modules/nwor_selector." DYNAMIC_LIBRARY_EXTENSION;
	::nwol::error_t											
	errMy														= ::loadPlatformValues(runtimeValues, defaultModuleName, __argv, __argc);		retval_error_if_errored(EXIT_FAILURE, errMy, "%s() failed.", "loadPlatformValues"	);  
	errMy														= ::rtMain(runtimeState);														retval_error_if_errored(EXIT_FAILURE, errMy, "%s() failed.", "rtMain"				);  
	return EXIT_SUCCESS;
}
#	endif
#endif
