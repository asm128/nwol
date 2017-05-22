#include "nwol_screen.h"
#include "nwol_log.h"

LRESULT								nwol::SScreen::WndProc						(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)	{
	sizeof(SScreenMetrics	);						
	sizeof(SScreenDetail	);
	sizeof(SScreenState		);
	sizeof(SScreen			);
	switch( uMsg ) {
	case WM_DESTROY:
		info_printf( "Received WM_DESTROY message. Posting WM_QUIT..." );
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
			//State.RequiresResizeWindow						= true;	? 
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
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
