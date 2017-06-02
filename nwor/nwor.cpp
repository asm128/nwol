#include "nwor.h"

#include "multithread.h"
#include "nwol_enum.h"

#if defined(__ANDROID__)
#	include <android/native_activity.h>
#elif defined(__LINUX__)
#	error "Not implemented."
#elif defined(__WINDOWS__)
#	include <process.h>
#else 
#	error "Platform not supported."
#endif

#include <thread>

::nwor::SRuntimeState												* g_RuntimeState						= nullptr;

void																renderLoop								(::nwor::SRuntimeState& stateRuntime)																					{
	info_printf("Render loop start.");
	static const char														* errorFormat1							= "Dynamically loaded function is null, maybe due to a buffer overrun which erased the pointers: %s.";
	static const char														* errorFormat2							= "Module function failed with code 0x%x: %s.";
	::nwol::SApplicationModule												& containerForCallbacks					= stateRuntime.MainModule;
	while(false == stateRuntime.Quit) {
		if(errored(containerForCallbacks.Render())) {
			error_printf("%s", "Fatal error rendering client module.");
			stateRuntime.Quit													= true;
		}
		::nwol::RUNTIME_CALLBACK_ID												callbackPointersErased					= containerForCallbacks.TestForNullPointerFunctions();
		if(callbackPointersErased) { 
			printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
			stateRuntime.Quit													= true; 
			break; 
		}
	}
	info_printf("Render loop end.");
}

void																renderThread							(void* pStateRuntime)																									{
	ret_error_if(0 == pStateRuntime, "Runtime state pointer is null. Thread function exiting...");
	info_printf("Render thread started.");

	::nwor::SRuntimeState													& stateRuntime							= *(::nwor::SRuntimeState*)pStateRuntime;
	INTERLOCKED_INCREMENT	(stateRuntime.RenderThreadUsers);
	renderLoop				(stateRuntime);
	INTERLOCKED_DECREMENT	(stateRuntime.RenderThreadUsers);

	info_printf("Render loop exited.");
}

int32_t																launchRenderThread						(::nwor::SRuntimeState& runtimeState)																					{
#if defined(__WINDOWS__)
	_beginthread(renderThread, 0, &runtimeState);
#else
#error "Not implemented."
#endif
	return 0;
}

LRESULT	WINAPI														mainWndProc								(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)	{
	::nwol::SScreen															& mainScreen							= g_RuntimeState->RuntimeValues.Screen;
	::nwol::SScreenState													& screenState							= mainScreen.State;
	error_if(errored(mainScreen.PlatformDetail.Messages.push_back({hWnd, uMsg, wParam, lParam})), "Out of memory?");
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
			screenState.MinOrMaxed = screenState.NoDraw							= true;
			break;
		}
		else if( wParam == SIZE_MAXIMIZED ) {
			info_printf("Received SIZE_MAXIMIZED.");
			screenState.RequiresResizeWindow = screenState.MinOrMaxed			= true;
			screenState.NoDraw													= false;
		}
		else if( wParam == SIZE_RESTORED ) {
			info_printf("Received SIZE_RESTORED.");
			screenState.RequiresResizeWindow = screenState.MinOrMaxed			= true;
			screenState.NoDraw													= false;
		}
		else {
			info_printf("Received unknown WM_SIZE message.");
			//State.RequiresResizeWindow						= true;
			screenState.MinOrMaxed = screenState.NoDraw							= false;
		}
		break;
	case WM_SHOWWINDOW:
	case WM_CREATE:
	case WM_EXITSIZEMOVE:
		info_printf("Received WM_EXITSIZEMOVE message.");
		screenState.NoDraw													= false;
		screenState.RequiresResizeWindow									= true;
		break;
	case WM_MENUCHAR:	// The WM_MENUCHAR message is sent when a menu is active and the user presses a key that does not correspond to any mnemonic or accelerator key. 
		return MAKELRESULT(0, MNC_CLOSE);	// Don't beep when we alt-enter.
		break;
	case WM_GETMINMAXINFO:	// Catch this message so to prevent the window from becoming too small.
		((MINMAXINFO*)lParam)->ptMinTrackSize.x								= 320;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y								= 200; 
		return 0;
	case WM_SYSKEYDOWN:
		return 0;
	case WM_CLOSE:
		info_printf( "Received WM_CLOSE message." );
		screenState.Closed													= true;
		return 0;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

int32_t																mainLoop								(::nwor::SRuntimeState & runtimeState, ::nwol::SApplicationModule& containerForCallbacks)								{
	typedef				uint8_t												RUNTIME_FLAG;
	static constexpr	const RUNTIME_FLAG									RUNTIME_FLAG_RUNNING					= 1;
	static constexpr	const RUNTIME_FLAG									RUNTIME_FLAG_NOT_YET_REQUESTED			= 2;

	RUNTIME_FLAG															executionState							= RUNTIME_FLAG_RUNNING | RUNTIME_FLAG_NOT_YET_REQUESTED;
	
	nwol_necall(launchRenderThread(runtimeState), "Failed to start render thread.");

	int32_t																	errLoop									= 0;
	MSG																		windowMessage							= {};
	HWND																	mainWindow								= runtimeState.RuntimeValues.Screen.PlatformDetail.hWnd;
	while(executionState & RUNTIME_FLAG_RUNNING) {
		while(PeekMessageA(&windowMessage, mainWindow, 0, 0, PM_REMOVE)) {
			TranslateMessage( &windowMessage );
			DispatchMessage	( &windowMessage );
		}
		while(PeekMessageA(&windowMessage, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage( &windowMessage );
			DispatchMessage	( &windowMessage );
		}
		::nwol::APPLICATION_STATE												errUpdate								= containerForCallbacks.Update(runtimeState.Quit || runtimeState.RuntimeValues.Screen.State.Closed); 
		runtimeState.RuntimeValues.Screen.State.Closed						= false;
		verbose_printf("Update return state for enum '%s': '%s' (0x%X).", ::nwol::genum_definition<::nwol::APPLICATION_STATE>::get().Name.c_str(), ::nwol::get_value_label(errUpdate).c_str());
		switch(errUpdate) {
		case ::nwol::APPLICATION_STATE_NORMAL: 
			break;
		case ::nwol::APPLICATION_STATE_EXIT :
			runtimeState.Quit													= true;													
			runtimeState.Processing												= false;												
			break;																	
		case ::nwol::APPLICATION_STATE_BUSY :
			runtimeState.Processing												= true;													
			break;																	
		case ::nwol::APPLICATION_STATE_STAY :
			executionState														|= ~RUNTIME_FLAG_NOT_YET_REQUESTED;
			runtimeState.Quit													= false;
			runtimeState.Processing												= true;					
			break;
		default:
			error_printf("Module function failed with code 0x%x: %s.", errUpdate, "moduleUpdate()"); 
			errLoop																= -1;
			runtimeState.Quit													= true;													
			runtimeState.Processing												= false;												
			break;
		}
		g_RuntimeState->RuntimeValues.Screen.PlatformDetail.Messages.clear();
		::nwol::RUNTIME_CALLBACK_ID												callbackPointersErased					= containerForCallbacks.TestForNullPointerFunctions();
		static constexpr	const char											errorFormat1[]							= "Dynamically loaded function is null, maybe due to a buffer overrun which erased the pointers: %s.";
		if(callbackPointersErased) { 
			::nwol::printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
			errLoop																= -1;
			runtimeState.Quit													= true; 
			break; 
		}

		if(runtimeState.Quit) {
			if(false == runtimeState.Processing) {
				executionState														&= ~RUNTIME_FLAG_RUNNING;
				info_printf("Execution shutting down...");
				break;
			}
			else if(executionState & RUNTIME_FLAG_NOT_YET_REQUESTED) {
				executionState														&= ~RUNTIME_FLAG_NOT_YET_REQUESTED;
				info_printf("Client module requested exit.");
			}
		}
	}
	return errLoop;
}

WNDCLASSEX															initWndClass							(HINSTANCE hInstance)																									{
	WNDCLASSEX																windowClass									;
	windowClass.cbSize													= sizeof(WNDCLASSEX)							;
	windowClass.style													= NULL											; // CS_HREDRAW | CS_VREDRAW;	;
	windowClass.lpfnWndProc												= mainWndProc									;
	windowClass.cbClsExtra												= 0												;
	windowClass.cbWndExtra												= 0												;
	windowClass.hInstance												= hInstance										;
	windowClass.hIcon													= LoadIcon(0, IDI_APPLICATION)					;
	windowClass.hCursor													= LoadCursor(0, IDC_ARROW)						;
	windowClass.hbrBackground											= CreateSolidBrush(GetSysColor(COLOR_3DFACE))	;
	windowClass.lpszMenuName											= 0												;
	windowClass.lpszClassName											= "nwor_screen"									;
	windowClass.hIconSm													= LoadIcon(0, IDI_WINLOGO)						;
	return windowClass;
}

int32_t																setupScreen								(::nwol::SRuntimeValues& runtimeValues, const char_t* windowTitle)														{ 
#if defined(__WINDOWS__)
	static	const WNDCLASSEX												windowClass								= runtimeValues.PlatformDetail.MainWindowClass = initWndClass(runtimeValues.PlatformDetail.hInstance);
	reterr_error_if(0 == RegisterClassExA(&runtimeValues.PlatformDetail.MainWindowClass), "Failed to register WNDCLASS \"%s\".", windowClass.lpszClassName);

	DWORD																	dwStyle									= WS_OVERLAPPED | WS_THICKFRAME | WS_BORDER | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;
	HWND																	newWindow								= CreateWindowExA
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

	runtimeValues.Screen.PlatformDetail	.pWindowClass					= &runtimeValues.PlatformDetail.MainWindowClass;
	runtimeValues.Screen.PlatformDetail	.hWnd							= newWindow;
	runtimeValues.PlatformDetail		.MainWindowStyle				= dwStyle;
	ShowWindow(newWindow, SW_SHOW);
#else
#	error "Not implemented"
#endif
	return 0;
}

int32_t																shutdownScreen							(::nwor::SRuntimeState& instanceApp)																					{ 
	::nwol::SRuntimeValues													& runtimeValues							= instanceApp.RuntimeValues;
	::nwol::SScreenDetail													& screenDetail							= runtimeValues.Screen.PlatformDetail;
	if( screenDetail.hWnd ) {
		::nwol::SScreenState													& screenState							= runtimeValues.Screen.State;
		HWND																	oldWindow								= screenDetail.hWnd;
		screenState.NoDraw													= true;
		screenDetail.hWnd													= 0;
		MSG																		windowMessage;
		info_printf("Calling DestroyWindow().");
		DestroyWindow( oldWindow );
		while(PeekMessageA(&windowMessage, oldWindow, 0, 0, PM_REMOVE)) {
			TranslateMessage( &windowMessage );
			DispatchMessageA( &windowMessage );
		}
		WNDCLASSEX																& mainWindowClass						= runtimeValues.PlatformDetail.MainWindowClass;
		bool_t																	bClassUnregistered						= (UnregisterClass(mainWindowClass.lpszClassName, mainWindowClass.hInstance) != 0) ? true : false;
		std::string																windowsError							= ::nwol::getWindowsErrorAsString(GetLastError());
		reterr_error_if(!bClassUnregistered, "Failed to unregister WNDCLASS \"%s\". \"%s\"", mainWindowClass.lpszClassName, windowsError.c_str());
	}
	return 0;
}

int																	nwor::rtMain							(::nwor::SRuntimeState& runtimeState)																					{
#if defined(__WINDOWS__) && defined(NWOL_DEBUG_ENABLED)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF );	// Enable run-time memory check for debug builds.
	//_CrtSetBreakAlloc( 322 );
#endif	

	::nwol::SRuntimeValues													& runtimeValues							= runtimeState.RuntimeValues;
	::nwol::SApplicationModule												preContainerForCallbacks				= {};
	preContainerForCallbacks.RuntimeValues								= &runtimeValues;
	nwol_necall(::nwol::applicationModuleLoad(runtimeValues, preContainerForCallbacks, runtimeValues.FileNameApplication), "Failed to load module %s.", runtimeValues.FileNameApplication);
	
	::nwol::SApplicationModule												& containerForCallbacks					= runtimeState.MainModule	= preContainerForCallbacks;
	char																	windowTitle[512]						= {};
	sprintf_s(windowTitle, "%s v%u.%u", containerForCallbacks.ModuleTitle, containerForCallbacks.VersionMajor(), containerForCallbacks.VersionMinor());
	reterr_error_if(0 > ::setupScreen(runtimeValues, windowTitle), "Failed to create main window.");

	static constexpr const char												errorFormat1	[]						= "Dynamically loaded function is null, maybe due to a buffer overrun which erased the pointers: %s.";
	static constexpr const char												errorFormat2	[]						= "Module function failed with code 0x%x: %s.";
	int32_t																	errCreate								= containerForCallbacks.Create(); 
	if(0 > errCreate) { 
		runtimeState.Quit													= true; 
		error_printf(errorFormat2, errCreate, "moduleCreate()"); 
	} 
	else {
		::nwol::RUNTIME_CALLBACK_ID												callbackPointersErased					= containerForCallbacks.TestForNullPointerFunctions();
		if(callbackPointersErased) { 
			printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
			runtimeState.Quit													= true; 
		}
		else {
			int32_t																	errSetup								= containerForCallbacks.Setup(); 
			if(0 > errSetup) { 
				runtimeState.Quit													= true; 
				error_printf(errorFormat2, errSetup, "moduleSetup()" ); 
			} 
			else {
				callbackPointersErased												= containerForCallbacks.TestForNullPointerFunctions();
				if(callbackPointersErased) { 
					printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
					runtimeState.Quit													= true; 
				}
				else {
					INTERLOCKED_INCREMENT(runtimeState.RenderThreadUsers);

					::nwol::error_t															errLoop									= mainLoop(runtimeState, containerForCallbacks);
					always_printf("Main loop exited with code %u.", (uint32_t)errLoop); 

					while(INTERLOCKED_COMPARE_EXCHANGE(runtimeState.RenderThreadUsers, 0, 1))
						::std::this_thread::sleep_for(::std::chrono::milliseconds(1));;
						
 					::nwol::error_t															errCleanup								= containerForCallbacks.Cleanup(); 
					if(0 > errCleanup) { 
						runtimeState.Quit													= true; 
						error_printf(errorFormat2, errCleanup, "moduleCleanup()"); 
					} 
					else {
						callbackPointersErased												= containerForCallbacks.TestForNullPointerFunctions();
						if(callbackPointersErased) { 
							printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
							runtimeState.Quit													= true; 
						}
					}
				}
			}
		}
#if defined(__WINDOWS__)
		PostQuitMessage(0);
		MSG																		windowMessage							= {};
		while(WM_QUIT != windowMessage.message) 
			while(PeekMessageA(&windowMessage, 0, 0, 0, PM_REMOVE)) {
				TranslateMessage( &windowMessage );
				DispatchMessage	( &windowMessage );
			}
#endif
		::nwol::error_t															errDelete								= containerForCallbacks.Delete(); 
		error_if(errored(errDelete), errorFormat2, errDelete, "moduleDelete()"); 

		callbackPointersErased												= containerForCallbacks.TestForNullPointerFunctions();
		if(callbackPointersErased) { 
			printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
			runtimeState.Quit													= true; 
		}
	}
	nwol_necall(::nwol::moduleUnload(containerForCallbacks)	, "Error unloading main module.");
	nwol_necall(::shutdownScreen(runtimeState)				, "Failed to shut down main screen/window.");
	return 0;
}
