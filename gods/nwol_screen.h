#include "runtime.h"

#include "gref_ptr.h"
#include "gref.h"
#include "nwol_coord.h"

#ifndef NWOL_WINDOW_H_9782365897236
#define NWOL_WINDOW_H_9782365897236

namespace nwol
{
#pragma pack(push, 1)
	struct SScreenState {
		bool												MinOrMaxed					: 1;
		bool												NoDraw						: 1;
		bool												RequiresResizeWindow		: 1;
		bool												Closed						: 1;
	};

	struct SScreenMetrics {
		::nwol::SCoord2<int16_t>							Position;
		::nwol::SCoord2<uint16_t>							Size;		
	};

	struct SScreenDetail {
#if defined(__WINDOWS__)
		const WNDCLASSEX									* pWindowClass				= nullptr;
		HWND												hWnd						= NULL;
#else
#	error	"Not implemented."
#endif
	};

#pragma pack(pop)

	struct SScreen {
		SScreenMetrics										Metrics						= {10, 10, 320, 240};
		SScreenState										State						= {};
		SScreenDetail										PlatformDetail				= {};
#if defined(__WINDOWS__)
		LRESULT												WndProc						(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
	};
	GDECLARE_OBJ(SScreen);

	::nwol::error_t										createScreen				(GPNCO(::nwol, SScreen)& createdScreen, ::nwol::SRuntimeValues&);

	struct SScreenManager {
		::nwol::array_obj<::nwol::SScreen>					Screens						;
		::nwol::error_t										CreateScreen				(const SScreenMetrics & desiredMetrics, id_t& screenIndex);
#if defined(__WINDOWS__)
		LRESULT												WndProc						(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)	{
			if(hWnd) {
				for(uint32_t iWindow = 0, windowCount = Screens.size(); iWindow < windowCount; ++iWindow) {
					::nwol::SScreen											& currentWindow				= Screens[iWindow];
					if(currentWindow.PlatformDetail.hWnd == hWnd)
						return currentWindow.WndProc(hWnd, uMsg, wParam, lParam);
				}
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
#endif
	};
}

#endif // NWOL_WINDOW_H_9782365897236
