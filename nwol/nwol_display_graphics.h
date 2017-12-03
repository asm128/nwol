#include "nwol_coord.h"
#include "nwol_array.h"
#include "nwol_ptr.h"
#include "nwol_display.h"

#if defined(__WINDOWS__)
#include "nwol_com_ptr.h"

#include <d3d11_4.h>
#include <dwrite_3.h>
#include <d3d11_4.h>
#include <d2d1_3.h>
#include <wincodec.h>
#endif

#ifndef NWOL_DISPLAY_GRAPHICS_H_29384623423
#define NWOL_DISPLAY_GRAPHICS_H_29384623423

namespace nwol
{
	struct SSwapChainDetail {
#if defined(__WINDOWS__)
							::nwol::com_ptr<::IDXGISwapChain1>					SwapChain										= {};
							::nwol::com_ptr<::ID3D11RenderTargetView>			RenderTargetView								= {};
		// --- Direct3D rendering objects. Required for 3D.
							::nwol::com_ptr<::ID3D11DepthStencilView>			DepthStencilView								= {};
							::nwol::com_ptr<::ID3D11DeviceContext2>				DeferredContext									= {};
#endif
	};

	// --- Direct2D rendering objects. Required for 2D.
	struct SD2D11 {
							::nwol::com_ptr<::ID2D1Factory2>					Factory											= {};
							::nwol::com_ptr<::ID2D1Device1>						Device											= {};
							::nwol::com_ptr<::ID2D1DeviceContext1>				Context											= {};
							::nwol::com_ptr<::ID2D1Bitmap1>						TargetBitmap									= {};
	};

	// --- DirectWrite & Windows Imaging Component objects.
	struct SDWrite {
							::nwol::com_ptr<::IDWriteFactory2>					DWriteFactory									= {};
	};

	struct SSwapChain {
							::nwol::SDisplay									* Display										= {};
							::nwol::SCoord2<uint32_t>							RenderTargetSize								= {};
							::nwol::SSwapChainDetail							PlatformDetail									= {};
	};

#if defined(__WINDOWS__)
	struct SD3D11 {
		// --- DirectX core objects. Required for 2D and 3D.
							::nwol::com_ptr<::IDXGIFactory2>					Factory											= {};
							::nwol::com_ptr<::ID3D11Device2>					Device											= {};
							::nwol::com_ptr<::ID3D11DeviceContext2>				Context											= {};
							::nwol::array_obj<::nwol::ptr_obj<SSwapChain>>		AttachedDisplays								= {};

		// --- Cached renderer properties.
							::D3D_FEATURE_LEVEL									FeatureLevel									= {};
							::nwol::SRectangle2D<float>							WindowBounds									= {};
							float												DPINext											= -1.0f;
							float												DPICurrent										= -1.0f;
							bool												CreateWindowSizeDependentResources				= true;
	};

	typedef	struct		::nwol::SD3D11										SRenderBaseDetail;

	struct SDX11 {
							::nwol::SD3D11										D3D												= {};
							::nwol::SD2D11										D2D												= {};
							::nwol::SDWrite										DWrite											= {};
	};
						::nwol::error_t										d2dCreateFactories							(::nwol::SDX11& instanceToInitialize);
						::nwol::error_t										dxCreateDevices								(::nwol::SDX11& instanceToInitialize);

	typedef				::nwol::SDX11										SGraphicsDetail;
#endif // __WINDOWS__	

	struct SGraphics {
							::nwol::SGraphicsDetail								PlatformDetail;
							::nwol::array_obj<::nwol::SSwapChain>				SwapChains;
	};

						::nwol::error_t										graphicsInitialize								(::nwol::SGraphics& instanceToInitialize);
						::nwol::error_t										graphicsAttach									(::nwol::SGraphics& instanceGraphics, ::nwol::SDisplay* displayToAttachTo);
} // namespace

#endif // NWOL_DISPLAY_GRAPHICS_H_29384623423
