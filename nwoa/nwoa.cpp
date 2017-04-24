#include "nwoa.h"

#include "text.h"
#include "ascii_screen.h"
#include "gui.h"

#include "runtime_impl.h"

#include <thread>
#include <process.h>

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(::SApplication, "No Workflow Overhead Application", 0, 1);

int32_t										cleanup									(::SApplication& instanceApp)							{ 
	::nwol::shutdownASCIIScreen();

	reterr_error_if(errored(::networkDisable(instanceApp)), "Error when disabling network.");

	return 0; 
}

void										printTypeDebug							()														{
	static const ::SApplication::TRegistry			memberRegistry							= ::SApplication::get_member_registry();
	for(uint32_t iMember=0; iMember < memberRegistry.get_member_count(); ++iMember) {
		const ::nwol::STypeIdentifier					& identifier						= memberRegistry.get_types()[iMember];
		always_printf
			(	"0x%X: %s::%s %s (%s): %s."
			,	memberRegistry.get_data_type_ids	()[iMember]
			,	identifier.NameSpace	.c_str()
			,	identifier.Name			.c_str()
			,	memberRegistry.get_names			()[iMember].c_str()
			,	memberRegistry.get_display_names	()[iMember].c_str()
			,	memberRegistry.get_descriptions		()[iMember].c_str()
			);	
	}

	::nwol::SApplicationNetworkClient::TRegistry			memberRegistryNet					= ::nwol::SApplicationNetworkClient::get_member_registry();
	for(uint32_t iMember=0; iMember < memberRegistry.get_member_count(); ++iMember) {
		const ::nwol::STypeIdentifier					& identifier						= memberRegistryNet.get_types()[iMember];
		always_printf
			(	"0x%X: %s::%s %s (%s): %s."
			,	memberRegistryNet.get_data_type_ids	()[iMember]
			,	identifier.NameSpace	.c_str()
			,	identifier.Name			.c_str()
			,	memberRegistryNet.get_names			()[iMember].c_str()
			,	memberRegistryNet.get_display_names	()[iMember].c_str()
			,	memberRegistryNet.get_descriptions	()[iMember].c_str()
			);
	}
}

LRESULT	WINAPI								mainWndProc								(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

::SApplication								* g_App									= nullptr;

int32_t										setupScreen								(::SApplication& instanceApp)							{ 
	g_App										= &instanceApp;
	WNDCLASSEX										windowClass;
	windowClass.cbSize							= sizeof(WNDCLASSEX);
	windowClass.style							= NULL; // CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc						= mainWndProc; 
	windowClass.cbClsExtra						= 0;
	windowClass.cbWndExtra						= 0;
	windowClass.hInstance						= instanceApp.RuntimeValues->PlatformDetail.hInstance;
	windowClass.hIcon							= LoadIcon(0, IDI_APPLICATION);
	windowClass.hCursor							= LoadCursor(0, IDC_ARROW);
	windowClass.hbrBackground					= CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	windowClass.lpszMenuName					= 0;
	windowClass.lpszClassName					= "nwoa_screen";
	windowClass.hIconSm							= LoadIcon(0, IDI_APPLICATION);
	DWORD											dwStyle									= WS_OVERLAPPED | WS_THICKFRAME | WS_BORDER | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;

	instanceApp.PlatformDetail.MainWindowClass	= windowClass;
	bool_t											bClassRegistered						= (RegisterClassEx(&instanceApp.PlatformDetail.MainWindowClass) != 0) ? true : false;
	reterr_error_if(!bClassRegistered, "Failed to register WNDCLASS \"%s\".", windowClass.lpszClassName);

	HWND											newWindow								= CreateWindowExA
		(	0L
		,	"nwoa_screen"
		,	appTitle()
		,	dwStyle
		,	instanceApp.Screen.Metrics.Position.x
		,	instanceApp.Screen.Metrics.Position.y
		,	instanceApp.Screen.Metrics.Size.x + GetSystemMetrics( SM_CXFRAME ) * 2
		,	instanceApp.Screen.Metrics.Size.y + GetSystemMetrics( SM_CYFRAME ) * 2 + GetSystemMetrics( SM_CYCAPTION )
		,	0, 0, windowClass.hInstance, 0
		); 
	reterr_error_if(0 == newWindow, "CreateWindow FAILED!");

	instanceApp.Screen.PlatformDetail	.hWnd				= newWindow;
	instanceApp.PlatformDetail			.MainWindowStyle	= dwStyle;
	return 0;
}

int32_t										setupGUI								(::SApplication& instanceApp)							{ 
	::nwol::SGUI									& guiSystem								= instanceApp.GUI;

	guiSystem.TargetSizeASCII.x					= ::nwol::getASCIIBackBufferWidth	();
	guiSystem.TargetSizeASCII.y					= ::nwol::getASCIIBackBufferHeight	();

	static const ::nwol::glabel						newControlLabel							= "Exit";
	::nwol::SGUIControl								newControl;

	newControl.AreaASCII						= {1, 1, (int32_t)newControlLabel.size(), 1}	;
	newControl.Text								= newControlLabel								;

	::nwol::error_t									errMy									= ::nwol::createControl(guiSystem, newControl);
	reterr_error_if_errored(errMy, "Failed to create control!");

	return 0;
}

int32_t										setup									(::SApplication& instanceApp)							{ 
	::nwol::SCoord2<uint32_t>						& sizeTargetASCII						= instanceApp.GUI.TargetSizeASCII;
	::nwol::initASCIIScreen(sizeTargetASCII.x, sizeTargetASCII.y);
	::nwol::setASCIIScreenTitle(appTitle());

	printTypeDebug();

	::nwol::error_t									
	errMy										= ::networkEnable	(instanceApp); reterr_error_if_errored(errMy, "Failed to enable network.");
	errMy										= ::setupGUI		(instanceApp); reterr_error_if_errored(errMy, "Failed to setup GUI.");
	errMy										= ::setupScreen		(instanceApp); reterr_error_if_errored(errMy, "Failed to setup screen.");

	return 0; 
}

#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif


int32_t										update									(::SApplication& instanceApp, bool exitRequested)		{
	if(exitRequested)
		return ::nwol::APPLICATION_STATE_EXIT;

	::nwol::SInput									& inputSystem							= instanceApp.Input;
	::nwol::pollInput(inputSystem);

	const ::nwol::SInput							& inputSystemConst						= instanceApp.Input;
	::nwol::SGUI									& guiSystem								= instanceApp.GUI;
	::nwol::updateGUI(guiSystem, inputSystemConst);

	::nwol::array_pod<::nwol::CONTROL_FLAG>			& controlFlags							= guiSystem.Controls.ControlFlags;

	for(uint32_t iControl = 0, controlCount = controlFlags.size(); iControl < controlCount; ++iControl)
		if(::nwol::bit_true(controlFlags[iControl], ::nwol::CONTROL_FLAG_EXECUTE)) {
			info_printf("Execute control %u.", iControl);
			switch(iControl) {
			case 0:		
				return ::nwol::APPLICATION_STATE_EXIT; 
				break;
			default:
				break;
			}
		}
	::nwol::SScreen		mainScreen;
	MSG					windowMessage;

	while(PeekMessageA(&windowMessage, instanceApp.Screen.PlatformDetail.hWnd, 0, 0, PM_REMOVE)) {
		TranslateMessage( &windowMessage );
		DispatchMessage	( &windowMessage );
	}

	if(windowMessage.message == WM_QUIT || instanceApp.Screen.State.Closed) {
		return ::nwol::APPLICATION_STATE_EXIT; 
	}

	return 0; 
}

int32_t														render									(::SApplication& instanceApp)							{ 
	::nwol::clearASCIIBackBuffer(' ', COLOR_WHITE);

	::nwol::SASCIITarget											target;
	::nwol::error_t													errMy									= ::nwol::getASCIIBackBuffer(target);
	reterr_error_if_errored(errMy, "%s", "Failed to get ASCII target!");
	
	::nwol::renderGUIASCII(target, instanceApp.GUI);

	::nwol::presentASCIIBackBuffer();
	
	return 0; 
}

LRESULT	WINAPI												mainWndProc								(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)	{
	::nwol::SScreenState											& screenState							= g_App->Screen.State;
	::nwol::SScreenDetail											& screenDetail							= g_App->Screen.PlatformDetail;
	switch( uMsg ) {
	case WM_DESTROY:
		info_printf( "Received WM_DESTROY message. Posting WM_QUIT..." );
		PostQuitMessage(0);
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
		info_printf( "Received WM_CLOSE message. Calling ShutdownApplication()..." );
		screenState.Closed											= true;
		return 0;
	}

	if(screenState.Closed) {
		if( screenDetail.hWnd ) {
			screenState.NoDraw											= true;
			HWND															oldWindow								= screenDetail.hWnd;
			screenDetail.hWnd											= 0;
			info_printf( "Calling DestroyWindow()." );
			DestroyWindow( oldWindow );
			return 0;
		}
	}
	else {
		if(screenState.RequiresResizeWindow ) {
			info_printf("Resizing window elements...");
			if(0) { //if( __resizeWindows() ) {
				warning_printf( "__resizeWindows() returned an error." );
			}
			else {
				screenState.RequiresResizeWindow							= false;
				info_printf( "__resizeWindows() succeeded." );
			}
		}
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

