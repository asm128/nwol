#include "nwol_runtime.h"
//#include "nwol_display.h"

#ifndef GSCREEN_H_092734908273498234__
#define GSCREEN_H_092734908273498234__

namespace nwol
{
	struct SDisplayManagerDetail {
#if defined(__WINDOWS__)
		WNDCLASSEX											WindowClass					;
#endif
	};

	struct SDisplayManager {
		SRuntimeValues										* RuntimeValues				= nullptr;
		SDisplayManagerDetail								PlatformDetail				= {};
		::nwol::array_obj<::nwol::SDisplay>					Screens						= {};

		inline constexpr									SDisplayManager				(SRuntimeValues* runtimeValues)									: RuntimeValues(runtimeValues) {}

		::nwol::error_t										CreateScreen				(const SRectangle2D<int16_t>& desiredMetrics, id_t& screenIndex);
#if defined(__WINDOWS__)
		LRESULT												WndProc						(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)			{
			rne_if(0 == hWnd, "Invalid window handle!");
			for(uint32_t iWindow = 0, windowCount = Screens.size(); iWindow < windowCount; ++iWindow) {
				::nwol::SDisplay											& currentWindow				= Screens[iWindow];
				if(currentWindow.PlatformDetail.hWnd == hWnd)
					return currentWindow.WndProc(hWnd, uMsg, wParam, lParam);
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
#endif
	};

	static inline ::nwol::error_t						initScreenManager			(SDisplayManager& screenManager, SRuntimeValues* runtimeValues)	{
		ree_if(0 == runtimeValues, "%s", "Cannot initialize SDisplayManager with a null SRuntimeValues*.");
		screenManager										= {runtimeValues};
#if defined(__WINDOWS__)
		screenManager.PlatformDetail.WindowClass;
#endif
		return 0;
	}
} // namespace

#endif // GSCREEN_H_092734908273498234__