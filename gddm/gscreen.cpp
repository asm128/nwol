#include "gscreen.h"

LRESULT	WINAPI										mainWndProc									(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)				{
//	::nwol::SDisplayManager										& windowManager				= ::nwol::SDisplayManager::getInstance();
//	return windowManager.WndProc(hWnd, uMsg, wParam, lParam);
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

struct SWindowClassRegistrator {
	::nwol::ptr_pod<WNDCLASSEX>							WindowClass									= {};
	ATOM												RegistrationError							= 0;

														~SWindowClassRegistrator					()																	{ UnregisterClass(WindowClass->lpszClassName, WindowClass->hInstance); }
														SWindowClassRegistrator						(WNDCLASSEX windowClass)											{
		::nwol::ref_create(&WindowClass, windowClass);
		RegistrationError									= RegisterClassEx(WindowClass);
	}		
};

static constexpr const TCHAR						runtimeWindowClassName[]					= TEXT("nwor_screen");

WNDCLASSEX											initWndClass								(HINSTANCE hInstance)												{
	WNDCLASSEX												windowClass									= {sizeof(WNDCLASSEX)};
	windowClass.style									= NULL											; // CS_HREDRAW | CS_VREDRAW;	;
	windowClass.lpfnWndProc								= mainWndProc									;
	windowClass.hInstance								= hInstance										;
	windowClass.hIcon									= LoadIcon(0, IDI_APPLICATION)					;
	windowClass.hCursor									= LoadCursor(0, IDC_ARROW)						;
	windowClass.hbrBackground							= CreateSolidBrush(GetSysColor(COLOR_3DFACE))	;
	windowClass.lpszClassName							= runtimeWindowClassName						;
	windowClass.hIconSm									= LoadIcon(0, IDI_APPLICATION)					;
	return windowClass;
}

::nwol::error_t										nwol::SDisplayManager::CreateScreen			(const SRectangle2D<int16_t>& desiredMetrics, id_t& screenIndex)	{
	static	const WNDCLASSEX								windowClass	= PlatformDetail.WindowClass	= initWndClass(RuntimeValues->PlatformDetail.EntryPointArgs.hInstance);

	static	const SWindowClassRegistrator					classRegistration							(PlatformDetail.WindowClass);
	if(0 == classRegistration.RegistrationError) {
		const DWORD												error										= GetLastError();
		error_printf("Cannot register window class: (0x%X) \"%s\"", (uint32_t)error, ::nwol::getWindowsErrorAsString(error).c_str());
		return -1;
	}
	const DWORD												dwStyle										= WS_OVERLAPPED | WS_THICKFRAME | WS_BORDER | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;
	const HWND												hWnd										= CreateWindowEx(0, PlatformDetail.WindowClass.lpszClassName, "Unnamed", dwStyle
		, desiredMetrics.Offset.x
		, desiredMetrics.Offset.y
		, desiredMetrics.Size.x
		, desiredMetrics.Size.y
		, 0, 0, PlatformDetail.WindowClass.hInstance, 0
		);
	::nwol::SDisplay											newScreen;
	newScreen.PlatformDetail.hWnd						= hWnd;
	newScreen.PlatformDetail.pWindowClass				= classRegistration.WindowClass;
	nwol_necall(screenIndex = Screens.push_back(newScreen), "%s", "Failed to push new screen to screen list!");
	return screenIndex;
}
