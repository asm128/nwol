#include "nwol_runtime.h"
#include "nwol_screen.h"

#ifndef GSCREEN_H_092734908273498234__
#define GSCREEN_H_092734908273498234__

namespace nwol
{
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
			retnul_error_if(0 == hWnd, "Invalid window handle!");
			for(uint32_t iWindow = 0, windowCount = Screens.size(); iWindow < windowCount; ++iWindow) {
				::nwol::SScreen											& currentWindow				= Screens[iWindow];
				if(currentWindow.PlatformDetail.hWnd == hWnd)
					return currentWindow.WndProc(hWnd, uMsg, wParam, lParam);
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
} // namespace

#endif // GSCREEN_H_092734908273498234__