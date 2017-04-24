#include "nwol_screen.h"
#include "glist.h"
#include "gref.h"
#include "gref_definition.h"

LRESULT								nwol::SScreen::WndProc						(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)	{
	sizeof(SScreenMetrics	);						
	sizeof(SScreenDetail	);
	sizeof(SScreenState		);
	sizeof(SScreen			);
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
			State.MinOrMaxed = State.NoDraw					= true;
			break;
		}
		else if( wParam == SIZE_MAXIMIZED ) {
			info_printf("Received SIZE_MAXIMIZED.");
			State.RequiresResizeWindow = State.MinOrMaxed	= true;
			State.NoDraw									= false;
		}
		else if( wParam == SIZE_RESTORED ) {
			info_printf("Received SIZE_RESTORED.");
			State.RequiresResizeWindow = State.MinOrMaxed	= true;
			State.NoDraw									= false;
		}
		else {
			info_printf("Received unknown WM_SIZE message.");
			//State.RequiresResizeWindow						= true;
			State.MinOrMaxed = State.NoDraw					= false;
		}

		break;
	case WM_SHOWWINDOW:
	case WM_CREATE:
	case WM_EXITSIZEMOVE:
		info_printf("Received WM_EXITSIZEMOVE message.");
		State.NoDraw									= false;
		State.RequiresResizeWindow						= true;
		break;
	case WM_MENUCHAR:	// The WM_MENUCHAR message is sent when a menu is active and the user presses a key that does not correspond to any mnemonic or accelerator key. 
		return MAKELRESULT(0, MNC_CLOSE);	// Don't beep when we alt-enter.
		break;
	case WM_GETMINMAXINFO:	// Catch this message so to prevent the window from becoming too small.
		((MINMAXINFO*)lParam)->ptMinTrackSize.x			= 320;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y			= 200; 
		return 0;
	case WM_SYSKEYDOWN:
		return 0;
	case WM_CLOSE:
		info_printf( "Received WM_CLOSE message. Calling ShutdownApplication()..." );
		State.Closed									= true;
		return 0;
	}

	if(State.RequiresResizeWindow) {
		info_printf("Resizing window elements...");
		if(0) { //if( __resizeWindows() ) {
			warning_printf( "__resizeWindows() returned an error." );
		}
		else {
			State.RequiresResizeWindow						= false;
			info_printf( "__resizeWindows() succeeded." );
		}
	}

	if(State.Closed) {
		if( PlatformDetail.hWnd ) {
			State.NoDraw									= true;
			HWND												oldWindow			= PlatformDetail.hWnd;
			PlatformDetail.hWnd								= 0;
			info_printf( "Calling DestroyWindow()." );
			DestroyWindow( oldWindow );
			return 0;
		}
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


LRESULT	WINAPI											mainWndProc					(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)	{
//	::nwol::SScreenManager										& windowManager				= ::nwol::SScreenManager::getInstance();
//	return windowManager.WndProc(hWnd, uMsg, wParam, lParam);
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

struct SWindowClassRegistrator {
	WNDCLASSEX							WindowClass								= {};
	ATOM								RegistrationError						= 0;

										SWindowClassRegistrator					(WNDCLASSEX windowClass)	: WindowClass(windowClass), RegistrationError(RegisterClassEx(&WindowClass)) {}		
										~SWindowClassRegistrator				()							{ UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance); }
};

::nwol::error_t						nwol::SScreenManager::CreateScreen			(const SScreenMetrics& desiredMetrics, id_t& screenIndex)		{
	SScreen										newScreen									;
	WNDCLASSEX									windowClass									;
	windowClass.cbSize						= sizeof(WNDCLASSEX);
	windowClass.style						= NULL											; // CS_HREDRAW | CS_VREDRAW;	;
	windowClass.lpfnWndProc					= mainWndProc									;
	windowClass.cbClsExtra					= 0												;
	windowClass.cbWndExtra					= 0												;
	windowClass.hInstance					= RuntimeValues->PlatformDetail.hInstance		;
	windowClass.hIcon						= LoadIcon(0, IDI_APPLICATION)					;
	windowClass.hCursor						= LoadCursor(0, IDC_ARROW)						;
	windowClass.hbrBackground				= CreateSolidBrush(GetSysColor(COLOR_3DFACE))	;
	windowClass.lpszMenuName				= 0												;
	windowClass.lpszClassName				= "nwol_system"									;
	windowClass.hIconSm						= LoadIcon(0, IDI_APPLICATION)					;
	//ATOM										result						= RegisterClassEx(&windowClass);
	static	const SWindowClassRegistrator		classRegistration(windowClass);
	if(0 == classRegistration.RegistrationError) {
		const DWORD									error						= GetLastError();
		const std::string							errorText					= ::nwol::getWindowsErrorAsString(error);
		error_printf("Cannot register window class: (0x%X) \"%s\"", (uint32_t)error, errorText.c_str());
		return -1;
	}

	const DWORD									dwStyle						= WS_OVERLAPPED | WS_THICKFRAME | WS_BORDER | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;
	const HWND									hWnd						= CreateWindowEx(0, windowClass.lpszClassName, "Unnamed", dwStyle
		, desiredMetrics.Position.x
		, desiredMetrics.Position.y
		, desiredMetrics.Size.x
		, desiredMetrics.Size.y
		, 0, 0, windowClass.hInstance, 0
		);
	
	newScreen.PlatformDetail.hWnd			= hWnd;
	newScreen.PlatformDetail.pWindowClass	= &classRegistration.WindowClass;

	uint32_t									expectedIndex				= Screens.size();
	uint32_t									newIndex					= Screens.push_back(newScreen);
	reterr_error_if(newIndex != expectedIndex, "%s", "Failed to push new screen to screen list!");
	return screenIndex = newIndex;
}
