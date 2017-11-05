/// Copyright 2017 - asm128
#include "nwol_coord.h"
#include "nwol_array.h"
#include "nwol_ptr.h"
#include "nwol_enum.h"
#if defined(__WINDOWS__)
#	include <Windows.h>
#endif

#ifndef NWOL_WINDOW_H_9782365897236
#define NWOL_WINDOW_H_9782365897236

namespace nwol
{
#pragma pack(push, 1)
	struct SDisplayMetrics {
		::nwol::SCoord2<int16_t>							Position;
		::nwol::SCoord2<uint16_t>							Size;		
	};

#if defined(__WINDOWS__)
	struct SWindowsMessage {
		HWND												hWnd; 
		UINT												uMsg; 
		WPARAM												wParam; 
		LPARAM												lParam;
	};
#endif
	struct SDisplayDetail {
#if defined(__WINDOWS__)
		HWND												hWnd						= NULL;		// This is also required for interaction with other APIs.
		::nwol::ptr_pod<WNDCLASSEX>							pWindowClass				= {};	// Store the window class used to create this window.
		::nwol::array_pod<SWindowsMessage>					Messages					= {};	// This queues the messages received by some custom wndproc to be processed by the subsystems such as input or gpu APIs. Also the TCP from winsock could probably make use of this.
		DWORD												Style						= WS_OVERLAPPED | WS_THICKFRAME | WS_BORDER | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;
#else
#	error	"Not implemented."
#endif
	};

	struct SDisplayState {
		bool												MinOrMaxed					: 1;	// Used to track something about the window resizing operation. Related to WM_RESTORE and WM_SIZE messages (I think).
		bool												NoDraw						: 1;	// 
		bool												RequiresResizeWindow		: 1;	
		bool												Closed						: 1;	// By closed we mean closed and not hidden or minimized.
	};

	GDEFINE_ENUM_TYPE(DISPLAY_EVENT, uint8_t);
	GDEFINE_ENUM_VALUE(DISPLAY_EVENT, RESTORE		,  1);
	GDEFINE_ENUM_VALUE(DISPLAY_EVENT, EXITSIZEMOVE	,  2);
	GDEFINE_ENUM_VALUE(DISPLAY_EVENT, SIZE			,  3);
	GDEFINE_ENUM_VALUE(DISPLAY_EVENT, CREATE		,  4);
	GDEFINE_ENUM_VALUE(DISPLAY_EVENT, CLOSE			,  5);
	GDEFINE_ENUM_VALUE(DISPLAY_EVENT, SHOW			,  6);
	GDEFINE_ENUM_VALUE(DISPLAY_EVENT, DESTROY		,  7);
	GDEFINE_ENUM_VALUE(DISPLAY_EVENT, SUSPEND		,  8);
	GDEFINE_ENUM_VALUE(DISPLAY_EVENT, ACTIVATE		,  9);
	GDEFINE_ENUM_VALUE(DISPLAY_EVENT, PLATFORM		, 10);
	GDEFINE_ENUM_VALUE(DISPLAY_EVENT, REMOVE		, 11);

#if defined(__WINDOWS__)
	typedef SWindowsMessage								SDisplayEventDetail;
#else
	struct SDisplayEventDetail {};
#endif
	struct SDisplayEvent {
		DISPLAY_EVENT										EventToHandle	;
		uint64_t											Input			;
		uint64_t											Output			;
		SDisplayEventDetail									PlatformDetail	;
	};
#pragma pack(pop)

	struct SDisplay;
	typedef	int											(*displayEventCallback)		(SDisplay& instanceDisplay, DISPLAY_EVENT eventToHandle);
	struct SDisplay {
		::nwol::array_obj<displayEventCallback>				CallbackList				= {};
		::nwol::array_obj<SDisplayEvent>					EventList					= {};
		::nwol::SDisplayState								State						= {};
		::nwol::SRectangle2D<int16_t>						Metrics						= {{10, 10}, {320, 240}};
		::nwol::SDisplayDetail								PlatformDetail				= {};
#if defined(__WINDOWS__)
		LRESULT												WndProc						(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
	};

}

#endif // NWOL_WINDOW_H_9782365897236
