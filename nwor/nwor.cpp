#include "nwor.h"

#include "nwol_multithread.h"
#include "nwol_enum.h"
#include "nwol_sleep.h"

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
			::nwol::printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
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
	::renderLoop			(stateRuntime);
	INTERLOCKED_DECREMENT	(stateRuntime.RenderThreadUsers);
	info_printf("Render loop exited.");
}

::nwol::error_t														launchRenderThread						(::nwor::SRuntimeState& runtimeState)																					{
#if defined(__WINDOWS__)
	::_beginthread(::renderThread, 0, &runtimeState);
#else
#error "Not implemented."
#endif
	return 0;
}

::nwol::error_t														mainLoop								(::nwor::SRuntimeState & runtimeState, ::nwol::SApplicationModule& containerForCallbacks)								{
	typedef				uint8_t												RUNTIME_FLAG;
	static constexpr	const RUNTIME_FLAG									RUNTIME_FLAG_RUNNING					= 1;
	static constexpr	const RUNTIME_FLAG									RUNTIME_FLAG_NOT_YET_REQUESTED			= 2;

	RUNTIME_FLAG															executionState							= RUNTIME_FLAG_RUNNING | RUNTIME_FLAG_NOT_YET_REQUESTED;
	nwol_necall(::launchRenderThread(runtimeState), "Failed to start render thread.");

	int32_t																	errLoop									= 0;
	MSG																		windowMessage							= {};
	HWND																	mainWindow								= runtimeState.RuntimeValues.Screen.PlatformDetail.hWnd;
	while(executionState & RUNTIME_FLAG_RUNNING) {
		while(::PeekMessage(&windowMessage, mainWindow, 0, 0, PM_REMOVE)) {
			::TranslateMessage( &windowMessage );
			::DispatchMessage( &windowMessage );
		}
		while(::PeekMessage(&windowMessage, 0, 0, 0, PM_REMOVE)) {
			::TranslateMessage( &windowMessage );
			::DispatchMessage( &windowMessage );
		}
		::nwol::APPLICATION_STATE												errUpdate								= containerForCallbacks.Update(runtimeState.Quit || runtimeState.RuntimeValues.Screen.State.Closed); 
		runtimeState.RuntimeValues.Screen.State.Closed						= false;
		verbose_printf("Update return state for enum '%s': '%s' (0x%X).", ::nwol::genum_definition<::nwol::APPLICATION_STATE>::get().Name.c_str(), ::nwol::get_value_label(errUpdate).c_str());
		switch(errUpdate) {
		case ::nwol::APPLICATION_STATE_NORMAL	: break;
		case ::nwol::APPLICATION_STATE_EXIT		:
			runtimeState.Quit													= true;													
			runtimeState.Processing												= false;												
			break;																	
		case ::nwol::APPLICATION_STATE_BUSY		:
			runtimeState.Processing												= true;													
			break;																		
		case ::nwol::APPLICATION_STATE_STAY		:
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
		runtimeState.RuntimeValues.Screen.PlatformDetail.Messages.clear();
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

static constexpr const TCHAR										runtimeWindowClassName	[]				= TEXT("nwor_screen");

LRESULT	WINAPI														mainWndProc								(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)																	{ return g_RuntimeState->RuntimeValues.Screen.WndProc(hWnd, uMsg, wParam, lParam); }
WNDCLASSEX															initWndClass							(HINSTANCE hInstance)																									{
	WNDCLASSEX																windowClass								= {sizeof(WNDCLASSEX)};
	windowClass.style													= NULL												; // CS_HREDRAW | CS_VREDRAW;	;
	windowClass.lpfnWndProc												= ::mainWndProc										;
	windowClass.hInstance												= hInstance											;
	windowClass.hIcon													= ::LoadIcon(0, IDI_APPLICATION)					;
	windowClass.hCursor													= ::LoadCursor(0, IDC_ARROW)						;
	windowClass.hbrBackground											= ::CreateSolidBrush(::GetSysColor(COLOR_3DFACE))	;
	windowClass.lpszClassName											= runtimeWindowClassName							;
	windowClass.hIconSm													= ::LoadIcon(0, IDI_WINLOGO)						;
	return windowClass;
}

::nwol::error_t														setupScreen								(::nwol::SRuntimeValues& runtimeValues, const char_t* windowTitle)														{ 
#if defined(__WINDOWS__)
	::nwol::ptr_pod<WNDCLASSEX>												windowClass								= {};
	windowClass.create(::initWndClass(runtimeValues.PlatformDetail.EntryPointArgs.hInstance));
	ree_if(0 == ::RegisterClassEx(windowClass), "Failed to register WNDCLASS \"%s\".", windowClass->lpszClassName);
	runtimeValues.Screen.PlatformDetail	.pWindowClass					= windowClass;

	DWORD																	dwStyle									= WS_OVERLAPPED | WS_THICKFRAME | WS_BORDER | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;
	HWND																	newWindow								= CreateWindowEx
		(	0L
		,	runtimeWindowClassName
		,	windowTitle
		,	dwStyle
		,	runtimeValues.Screen.Metrics.Offset.x
		,	runtimeValues.Screen.Metrics.Offset.y
		,	runtimeValues.Screen.Metrics.Size.x + ::GetSystemMetrics( SM_CXFRAME ) * 2
		,	runtimeValues.Screen.Metrics.Size.y + ::GetSystemMetrics( SM_CYFRAME ) * 2 + ::GetSystemMetrics( SM_CYCAPTION )
		,	0, 0, windowClass->hInstance, 0
		); 
	ree_if(0 == newWindow, "CreateWindow FAILED!");
	runtimeValues.Screen.PlatformDetail	.Style							= dwStyle;
	runtimeValues.Screen.PlatformDetail	.hWnd							= newWindow;
	::ShowWindow(newWindow, SW_SHOWNORMAL);
#else
#	error "Not implemented"
#endif
	return 0;
}

::nwol::error_t														shutdownScreen							(::nwol::SRuntimeValues & runtimeValues)																				{ 
	::nwol::SDisplayDetail													& screenDetail							= runtimeValues.Screen.PlatformDetail;
	if( screenDetail.hWnd ) {
		::nwol::SDisplayState													& screenState							= runtimeValues.Screen.State;
		::HWND																	oldWindow								= screenDetail.hWnd;
		screenState.NoDraw													= true;
		screenDetail.hWnd													= 0;
		::MSG																	windowMessage;
		info_printf("Calling DestroyWindow().");
		::DestroyWindow( oldWindow );
		while(PeekMessage(&windowMessage, oldWindow, 0, 0, PM_REMOVE)) {
			::TranslateMessage( &windowMessage );
			::DispatchMessage( &windowMessage );
		}
		::WNDCLASSEX															& mainWindowClass						= *runtimeValues.Screen.PlatformDetail.pWindowClass;
		bool_t																	bClassUnregistered						= (::UnregisterClass(mainWindowClass.lpszClassName, mainWindowClass.hInstance) != 0) ? true : false;
		::std::string															windowsError							= ::nwol::getWindowsErrorAsString(GetLastError());
		ree_if(!bClassUnregistered, "Failed to unregister WNDCLASS \"%s\". \"%s\"", mainWindowClass.lpszClassName, windowsError.c_str());
	}
	return 0;
}

::nwol::error_t														nwor::rtMain							(::nwor::SRuntimeState& runtimeState)																					{
#if defined(__WINDOWS__) && defined(NWOL_DEBUG_ENABLED)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF );	// Enable run-time memory check for debug builds.
	//_CrtSetBreakAlloc( 436 );
#endif	

	::nwol::SRuntimeValues													& runtimeValues							= runtimeState.RuntimeValues;
	::nwol::SApplicationModule												preContainerForCallbacks				= {};
	preContainerForCallbacks.RuntimeValues								= &runtimeValues;
	nwol_necall(::nwol::applicationModuleLoad(runtimeValues, preContainerForCallbacks, runtimeValues.FileNameApplication.begin()), "Failed to load module %s.", runtimeValues.FileNameApplication.begin());
	
	::nwol::SApplicationModule												& containerForCallbacks					= runtimeState.MainModule	= preContainerForCallbacks;
	char																	windowTitle[512]						= {};
	sprintf_s(windowTitle, "%s v%u.%u", containerForCallbacks.ModuleTitle, containerForCallbacks.VersionMajor(), containerForCallbacks.VersionMinor());
	ree_if(0 > ::setupScreen(runtimeValues, windowTitle), "Failed to create main window.");

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
			::nwol::printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
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
					::nwol::printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
					runtimeState.Quit													= true; 
				}
				else {
					INTERLOCKED_INCREMENT(runtimeState.RenderThreadUsers);

					::nwol::error_t															errLoop									= mainLoop(runtimeState, containerForCallbacks);
					always_printf("Main loop exited with code %u.", (uint32_t)errLoop); 

					while(INTERLOCKED_COMPARE_EXCHANGE(runtimeState.RenderThreadUsers, 0, 1))
						::nwol::sleep(1);
						
 					::nwol::error_t															errCleanup								= containerForCallbacks.Cleanup(); 
					if(0 > errCleanup) { 
						runtimeState.Quit													= true; 
						error_printf(errorFormat2, errCleanup, "moduleCleanup()"); 
					} 
					else {
						callbackPointersErased												= containerForCallbacks.TestForNullPointerFunctions();
						if(callbackPointersErased) { 
							::nwol::printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
							runtimeState.Quit													= true; 
						}
					}
				}
			}
		}
#if defined(__WINDOWS__)
		::PostQuitMessage(0);
		//MSG																		windowMessage							= {};
		//while(WM_QUIT != windowMessage.message) 
		//	while(PeekMessage(&windowMessage, 0, 0, 0, PM_REMOVE)) {
		//		TranslateMessage( &windowMessage );
		//		DispatchMessage	( &windowMessage );
		//	}
#endif
		::nwol::error_t															errDelete								= containerForCallbacks.Delete(); 
		error_if(errored(errDelete), errorFormat2, errDelete, "moduleDelete()"); 

		callbackPointersErased												= containerForCallbacks.TestForNullPointerFunctions();
		if(callbackPointersErased) { 
			::nwol::printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
			runtimeState.Quit													= true; 
		}
	}
	nwol_necall(::nwol::moduleUnload(containerForCallbacks)		, "Error unloading main module.");
	nwol_necall(::shutdownScreen(runtimeState.RuntimeValues)	, "Failed to shut down main screen/window.");
	return 0;
}
