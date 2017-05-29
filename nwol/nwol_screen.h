#include "nwol_coord.h"
#include "nwol_array.h"
#if defined(__WINDOWS__)
#	include <Windows.h>
#endif

#ifndef NWOL_WINDOW_H_9782365897236
#define NWOL_WINDOW_H_9782365897236

namespace nwol
{
#pragma pack(push, 1)
	struct SScreenMetrics {
		::nwol::SCoord2<int16_t>							Position;
		::nwol::SCoord2<uint16_t>							Size;		
	};

	struct SWindowsMessage {
		HWND	hWnd; 
		UINT	uMsg; 
		WPARAM	wParam; 
		LPARAM	lParam;
	};

	struct SScreenDetail {
#if defined(__WINDOWS__)
		const WNDCLASSEX									* pWindowClass				= nullptr;
		HWND												hWnd						= NULL;
		::nwol::array_pod<::nwol::SWindowsMessage>			Messages;
#else
#	error	"Not implemented."
#endif
	};

	struct SScreenState {
		bool												MinOrMaxed					: 1;
		bool												NoDraw						: 1;
		bool												RequiresResizeWindow		: 1;
		bool												Closed						: 1;
	};

	struct SScreen {
		::nwol::SScreenMetrics								Metrics						= {{10, 10}, {320, 240}};
		::nwol::SScreenDetail								PlatformDetail				= {};
		::nwol::SScreenState								State						= {};

#if defined(__WINDOWS__)
		LRESULT												WndProc						(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
	};

#pragma pack(pop)
}

#endif // NWOL_WINDOW_H_9782365897236
