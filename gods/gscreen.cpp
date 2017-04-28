#include "gscreen.h"


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
