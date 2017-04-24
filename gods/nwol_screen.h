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
		SScreenMetrics										Metrics						= {{10, 10}, {320, 240}};
		SScreenDetail										PlatformDetail				= {};
		SScreenState										State						= {};
#if defined(__WINDOWS__)
		LRESULT												WndProc						(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
	};
	GDECLARE_OBJ(SScreen);

	::nwol::error_t										createScreen				(GPNCO(::nwol, SScreen)& createdScreen, ::nwol::SRuntimeValues&);

	struct SScreenManagerDetail {
#if defined(__WINDOWS__)
		WNDCLASSEX											WindowClass					;
#endif
	};

	struct SScreenManager {
		SRuntimeValues										* RuntimeValues				= nullptr;
		SScreenManagerDetail								PlatformDetail				= {};
		::nwol::array_obj<::nwol::SScreen>					Screens						= {};

		inline constexpr									SScreenManager				(SRuntimeValues* runtimeValues)									: RuntimeValues(runtimeValues) {}

		::nwol::error_t										CreateScreen				(const SScreenMetrics & desiredMetrics, id_t& screenIndex);
#if defined(__WINDOWS__)
		LRESULT												WndProc						(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)			{
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

	static inline ::nwol::error_t						initScreenManager			(SScreenManager& screenManager, SRuntimeValues* runtimeValues)	{
		reterr_error_if(0 == runtimeValues, "%s", "Cannot initialize SScreenManager with a null SRuntimeValues*.");
		screenManager										= {runtimeValues};
#if defined(__WINDOWS__)
		screenManager.PlatformDetail.WindowClass;
#endif
		return 0;
	}
}

#endif // NWOL_WINDOW_H_9782365897236
