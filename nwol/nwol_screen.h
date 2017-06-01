/// Copyright 2017 - asm128
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


	struct SScreenDetail {
#if defined(__WINDOWS__)
		struct SWindowsMessage {
			HWND												hWnd; 
			UINT												uMsg; 
			WPARAM												wParam; 
			LPARAM												lParam;
		};
		const WNDCLASSEX									* pWindowClass				= nullptr;	// Store the window class used to create this window.
		HWND												hWnd						= NULL;		// This is also required for interaction with other APIs.
		::nwol::array_pod<SWindowsMessage>					Messages;	// This queues the messages received by some custom wndproc to be processed by the subsystems such as input or gpu APIs. Also the TCP from winsock could probably make use of this.
#else
#	error	"Not implemented."
#endif
	};

	struct SScreenState {
		bool												MinOrMaxed					: 1;	// Used to track something about the window resizing operation. Related to WM_RESTORE and WM_SIZE messages (I think).
		bool												NoDraw						: 1;	// 
		bool												RequiresResizeWindow		: 1;	
		bool												Closed						: 1;	// By closed we mean closed and not hidden or minimized.
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
