/// Copyright 2010-2017 - asm128
#include "nwol_display.h"
#include "nwol_log.h"

#if defined(__WINDOWS__)
LRESULT											nwol::SDisplay::WndProc						(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)	{
	verbose_printf("Received window message. hWnd = %p, uMsg = %i, wParam = %i, lParam = %i.", hWnd, (int)uMsg, (int)wParam, (int)lParam);
	error_if(errored(PlatformDetail.Messages.push_back({hWnd, uMsg, wParam, lParam})), "Out of memory?");
	switch( uMsg ) {
	case WM_MENUCHAR		: return MAKELRESULT(0, MNC_CLOSE); // The WM_MENUCHAR message is sent when a menu is active and the user presses a key that does not correspond to any mnemonic or accelerator key. Don't beep when we alt-enter.
	case WM_SYSKEYDOWN		: info_printf("Received WM_SYSKEYDOWN message."								); return 0;
	case WM_DESTROY			: info_printf("Received WM_DESTROY message. Doing nothing..."				); return 0;
	case WM_QUIT			: info_printf("Received WM_QUIT message."									); break;
	case WM_GETMINMAXINFO	: info_printf("Received WM_GETMINMAXINFO message."							); ((MINMAXINFO*)lParam)->ptMinTrackSize = {320, 200}; return 0;	// Catch this message so to prevent the window from becoming too small.
	case WM_CLOSE			: info_printf("Received WM_CLOSE message."									); State.NoDraw = State.Closed		= true; return 0;
	case WM_ENTERSIZEMOVE	: info_printf("Received WM_ENTERSIZEMOVE message."							); State.NoDraw = State.Resizing	= true; break;
	case WM_SHOWWINDOW		: info_printf("Received WM_SHOWWINDOW message. Fallback to WM_CREATE."		);
	case WM_CREATE			: info_printf("Received WM_CREATE message. Fallback to WM_EXITSIZEMOVE."	);
	case WM_EXITSIZEMOVE	: info_printf("Received WM_EXITSIZEMOVE message."							); State.Resized = !(State.NoDraw = State.Resizing = false); break;
	case WM_SIZE			:
		if(lParam) {
			::nwol::SCoord2<int16_t>							newMetrics									= ::nwol::SCoord2<WORD>{LOWORD(lParam), HIWORD(lParam)}.Cast<int16_t>();
			if(newMetrics != Metrics.Size) {
				Metrics.Size									= newMetrics;
				State.Resized									= true;
			}
		}
		if( wParam == SIZE_MINIMIZED ) {
			info_printf("SIZE_MINIMIZED.");
			State.MinOrMaxed = State.NoDraw					= true;
		}
		else if( wParam == SIZE_MAXIMIZED ) {
			info_printf("SIZE_MAXIMIZED.");
			State.Resized = State.MinOrMaxed				= true;
			State.NoDraw									= false;
		}
		else if( wParam == SIZE_RESTORED ) {
			info_printf("SIZE_RESTORED.");
			State.Resized = State.MinOrMaxed				= true;
			State.NoDraw									= false;
		}
		else {
			info_printf("Received unknown WM_SIZE message: 0x%p", wParam);
			//State.Resized									= true;	? 
			State.MinOrMaxed = State.NoDraw					= false;
		}
		break;
	}
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
#endif