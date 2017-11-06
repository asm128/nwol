#include "nwol_display_graphics.h"

// Create the Direct2D device object and a corresponding context.
static ::nwol::error_t										d2dCreateDeviceAndContext				(::nwol::SD3D11& d3d, ::nwol::SD2D11& d2dToInitialize)	{
	::nwol::com_ptr<::IDXGIDevice>									dxgiDevice								= {};
	nwol_necall(d3d.Device.as(&dxgiDevice), "Why would this happen?");	// Get the underlying DXGI device of the Direct3D device.
	nwol_hrcall(d2dToInitialize.Factory->CreateDevice(dxgiDevice, &d2dToInitialize.Device));	
	nwol_hrcall(d2dToInitialize.Device->CreateDeviceContext(::D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dToInitialize.Context));
	return 0;
}

static inline	::nwol::error_t								d3dSDKAvailable							()														{
	nwol_hrcall(::D3D11CreateDevice
		( nullptr
		, ::D3D_DRIVER_TYPE_NULL		// There is no need to create a real hardware device.
		, 0
		, ::D3D11_CREATE_DEVICE_DEBUG	// Check for the SDK layers.
		, nullptr						// Any feature level will do.
		, 0
		, D3D11_SDK_VERSION				// Always set this to D3D11_SDK_VERSION for Windows Store apps.
		, nullptr						// No need to keep the D3D device reference.
		, nullptr						// No need to know the feature level.
		, nullptr						// No need to keep the D3D device context reference.
		));
	return 0;
}

static ::nwol::error_t										d3dCreateDevice							(::nwol::SD3D11& d3dToInitialize)							{
	UINT															creationFlags							= ::D3D11_CREATE_DEVICE_BGRA_SUPPORT;	// This flag adds support for surfaces with a different color channel ordering than the API default. It is required for compatibility with Direct2D.
	::nwol::com_ptr<::IDXGIDevice>									dxgiDevice								= {};
#if defined(_DEBUG)
	if not_errored(::d3dSDKAvailable())
		creationFlags												|= ::D3D11_CREATE_DEVICE_DEBUG;	// If the project is in a debug build, enable debugging via SDK Layers with this flag.
#endif
	::D3D_FEATURE_LEVEL												featureLevels[]							=	
		{ ::D3D_FEATURE_LEVEL_11_1	// This array defines the set of DirectX hardware feature levels this app will support. Note the ordering should be preserved.
		, ::D3D_FEATURE_LEVEL_11_0	// Don't forget to declare your application's minimum required feature level in its description.
		, ::D3D_FEATURE_LEVEL_10_1	// All applications are assumed to support 9.1 unless otherwise stated.
		, ::D3D_FEATURE_LEVEL_10_0
		, ::D3D_FEATURE_LEVEL_9_3
		, ::D3D_FEATURE_LEVEL_9_2
		, ::D3D_FEATURE_LEVEL_9_1
		};

	// --- Create the Direct3D 11 API device object and a corresponding context.
	::nwol::com_ptr<::ID3D11Device>									device;
	::nwol::com_ptr<::ID3D11DeviceContext>							context;
	nwol_hrcall(::D3D11CreateDevice
		( nullptr						// Specify nullptr to use the default adapter.
		, ::D3D_DRIVER_TYPE_HARDWARE
		, 0
		, creationFlags					// Set debug and Direct2D compatibility flags.
		, featureLevels					// List of feature levels this app can support.
		, ARRAYSIZE(featureLevels)
		, D3D11_SDK_VERSION				// Always set this to D3D11_SDK_VERSION for Windows Store apps.
		, &device						// Returns the Direct3D device created.
		, &d3dToInitialize.FeatureLevel	// Returns feature level of device created.
		, &context						// Returns the device immediate context.
		));

	// --- Get the Direct3D 11.1 API device and context interfaces.
	ree_if(errored(device	.as(&d3dToInitialize.Device )), "Failed to acquire interface. Feature level not supported?");
	ree_if(errored(context	.as(&d3dToInitialize.Context)), "Failed to acquire interface. Why would this ever happen?");
	return 0;
}

::nwol::error_t												nwol::d2dInitializeFactories			(::nwol::SDX11& instanceToInitialize)															{ 
	::D2D1_FACTORY_OPTIONS											options									= {};
#if defined(_DEBUG)
	options.debugLevel											= ::D2D1_DEBUG_LEVEL_INFORMATION;	// If the project is in a debug build, enable Direct2D debugging via SDK Layers.
#endif
	nwol_hrcall(::D2D1CreateFactory		(::D2D1_FACTORY_TYPE_SINGLE_THREADED	, __uuidof(::ID2D1Factory2		), &options, (void**)&instanceToInitialize.D2D.Factory)			);
	nwol_hrcall(::DWriteCreateFactory	(::DWRITE_FACTORY_TYPE_SHARED			, __uuidof(::IDWriteFactory2	), (::IUnknown**)&instanceToInitialize.DWrite.DWriteFactory)	);
	return 0; 
}

::nwol::error_t												nwol::dxInitializeDevices				(::nwol::SDX11& instanceToInitialize)															{ 
	nwol_necall(::d3dCreateDevice			(instanceToInitialize.D3D), "Unknown reason.");
	nwol_necall(::d2dCreateDeviceAndContext	(instanceToInitialize.D3D, instanceToInitialize.D2D), "Unknown reason.");
	return 0; 
}

::nwol::error_t												nwol::graphicsInitialize				(::nwol::SGraphics& instanceToInitialize)														{
	nwol_necall(::nwol::d2dInitializeFactories	(instanceToInitialize.PlatformDetail), "Unknown reason");
	nwol_necall(::nwol::dxInitializeDevices		(instanceToInitialize.PlatformDetail), "Unknown reason");
	return 0;
}

::nwol::error_t												createWindowSizeDependentResources		(::nwol::SGraphics& instanceApplication)														{
	// Store the window bounds so the next time we get a SizeChanged event we can avoid rebuilding everything if the size is identical.
#ifdef TODO
	instanceApplication.WindowBounds							= m_window->Bounds;
#endif
	 nwol_necall(::nwol::graphicsAttach(instanceApplication, instanceApplication.SwapChains[0].Display), "Failed! Maybe the device was removed or something that prevents from creating the swapchain or the render target.");
	 return 0;
}

::nwol::error_t												updateForWindowSizeChange				(::nwol::SGraphics& instanceGraphics, ::nwol::SSwapChain& swapChain,const ::nwol::SCoord2<uint32_t>& currentSize)				{
	::nwol::SD3D11													& d3d									= instanceGraphics.PlatformDetail.D3D;
	if (d3d.DPINext != d3d.DPICurrent)	// Only handle window size changed if there is no pending DPI change.
		return 1;

	::nwol::SD2D11													& d2d									= instanceGraphics.PlatformDetail.D2D;
	if (currentSize.x != d3d.WindowBounds.Size.x 
	 || currentSize.y != d3d.WindowBounds.Size.y
	 )
	{
		d2d.Context->SetTarget(nullptr);
		d2d.TargetBitmap											= {};
		swapChain.PlatformDetail.RenderTargetView					= {};
		swapChain.PlatformDetail.DepthStencilView					= {};
		d3d.WindowSizeChangeInProgress								= true;
		nwol_necall(::createWindowSizeDependentResources(instanceGraphics), "Unknown error.");
		d3d.WindowBounds.Size										= currentSize.Cast<float>();
	}
	return 0;
}

::nwol::error_t												setDpi									(::nwol::SGraphics& instanceGraphics, ::nwol::SSwapChain& swapChain, const ::nwol::SCoord2<uint32_t>& currentSize, float dpi)	{
	::nwol::SD3D11													& d3d									= instanceGraphics.PlatformDetail.D3D;
	if (dpi != d3d.DPICurrent) {
		d3d.DPICurrent												= dpi;
		instanceGraphics.PlatformDetail.D2D.Context->SetDpi(dpi, dpi);	// Update Direct2D's stored DPI.
		d3d.DPINext																	= instanceGraphics.PlatformDetail.D3D.DPICurrent;
		::updateForWindowSizeChange(instanceGraphics, swapChain, currentSize);	// Often a DPI change implies a window size change. In some cases Windows will issue both a size changed event and a DPI changed event. In this case, the resulting bounds will not change, and the window resize code will only be executed once.
	}
	return 0;
}

::nwol::error_t												handleDeviceLost						(::nwol::SGraphics& instanceGraphics)															{
	::nwol::SD3D11													& d3d									= instanceGraphics.PlatformDetail.D3D;
	float															dpi										= d3d.DPICurrent;
	{ // Reset these member variables to ensure that SetDpi recreates all resources.
		d3d.DPICurrent											= -1.0f;
		d3d.WindowBounds.Size									= {};
		for(uint32_t iAttachedDisplay = 0; iAttachedDisplay < d3d.AttachedDisplays.size(); ++iAttachedDisplay)
			d3d.AttachedDisplays[iAttachedDisplay]->PlatformDetail.SwapChain	= {};
	}
	nwol_necall(::nwol::dxInitializeDevices(instanceGraphics.PlatformDetail), "Failed to create DirectX device.");
	for(uint32_t iAttachedDisplay = 0; iAttachedDisplay < d3d.AttachedDisplays.size(); ++iAttachedDisplay)
		::setDpi(instanceGraphics, *d3d.AttachedDisplays[iAttachedDisplay], {}, dpi);
	return 0;
}

::nwol::error_t												d3dAttachDisplay						(::nwol::SD3D11& d3d, ::nwol::SSwapChain& swapChain, ::nwol::SDisplay* displayToAttachTo)						{
	::nwol::SSwapChainDetail										& swapchainDetail						= swapChain.PlatformDetail;
	if (swapchainDetail.SwapChain != nullptr) {
		::HRESULT														hr										= swapchainDetail.SwapChain->ResizeBuffers(2, 0, 0, ::DXGI_FORMAT_B8G8R8A8_UNORM, 0);	// If the swap chain already exists, resize it.
		rve_if(1, hr == DXGI_ERROR_DEVICE_REMOVED, "Device was removed.");
		nwol_hrcall(hr);
	}
	else { // Otherwise, create a new one using the same adapter as the existing Direct3D device.
	    ::DXGI_SWAP_CHAIN_DESC1											swapChainDesc							= {0};
	    swapChainDesc.Width											= 0;								// Use automatic sizing.
	    swapChainDesc.Height										= 0;
	    swapChainDesc.Format										= ::DXGI_FORMAT_B8G8R8A8_UNORM;		// This is the most common swap chain format.
	    swapChainDesc.Stereo										= false;
	    swapChainDesc.SampleDesc.Count								= 1;								// Don't use multi-sampling.
	    swapChainDesc.SampleDesc.Quality							= 0;
	    swapChainDesc.BufferUsage									= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	    swapChainDesc.BufferCount									= 2;								// Use double-buffering to minimize latency.
	    swapChainDesc.Scaling										= ::DXGI_SCALING_NONE;
	    swapChainDesc.SwapEffect									= ::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	// All Windows Store apps must use this SwapEffect.
	    swapChainDesc.Flags											= 0;

	    ::nwol::com_ptr<::IDXGIDevice1>									dxgiDevice;
	    ::nwol::com_ptr<::IDXGIAdapter>									dxgiAdapter;
	    ::nwol::com_ptr<::IDXGIFactory2>								dxgiFactory;
		nwol_necall(d3d.Device.as(&dxgiDevice), "Failed to acquire COM interface!");
		nwol_hrcall(dxgiDevice->GetAdapter(&dxgiAdapter));
		nwol_hrcall(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));
		nwol_hrcall(dxgiFactory->CreateSwapChainForHwnd					
			( d3d.Device
			, displayToAttachTo->PlatformDetail.hWnd
			, &swapChainDesc, nullptr, nullptr
			, &swapchainDetail.SwapChain
			));
		nwol_hrcall(dxgiDevice->SetMaximumFrameLatency(1));	// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and ensures that the application will only render after each VSync, minimizing power consumption.
	}
	// --- Create a Direct3D render target view of the swap chain back buffer.
	::nwol::com_ptr<::ID3D11Texture2D>								backBuffer;
	nwol_hrcall(swapchainDetail.SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
	nwol_hrcall(d3d.Device->CreateRenderTargetView(backBuffer, nullptr, &swapchainDetail.RenderTargetView));

	// --- Cache the render target dimensions in our helper class for convenient use.
	::D3D11_TEXTURE2D_DESC											backBufferDesc							= {0};
	backBuffer->GetDesc(&backBufferDesc);
	d3d.RenderTargetSize										= {backBufferDesc.Width, backBufferDesc.Height};

	// --- Create a depth stencil view for use with 3D rendering if needed.
	::CD3D11_TEXTURE2D_DESC											depthStencilDesc						(::DXGI_FORMAT_D24_UNORM_S8_UINT, backBufferDesc.Width, backBufferDesc.Height, 1, 1, ::D3D11_BIND_DEPTH_STENCIL);
	::nwol::com_ptr<::ID3D11Texture2D>								depthStencil							= {};
	nwol_hrcall(d3d.Device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil));
	::CD3D11_DEPTH_STENCIL_VIEW_DESC								viewDesc								= ::CD3D11_DEPTH_STENCIL_VIEW_DESC(::D3D11_DSV_DIMENSION_TEXTURE2D);
	nwol_hrcall(d3d.Device->CreateDepthStencilView(depthStencil, &viewDesc, &swapchainDetail.DepthStencilView));

	// --- Set the 3D rendering viewport to target the entire window.
	::CD3D11_VIEWPORT												viewport								(0.0f, 0.0f, (float)backBufferDesc.Width, (float)backBufferDesc.Height);
	d3d.Context->RSSetViewports(1, &viewport);
	return 0;
}

::nwol::error_t												nwol::graphicsAttach					(::nwol::SGraphics& instanceGraphics, ::nwol::SDisplay* displayToAttachTo)						{
	ree_if(0 == displayToAttachTo, "Cannot attach to a null display.");
	::nwol::SD3D11													& d3d									= instanceGraphics.PlatformDetail.D3D;
	for(uint32_t iAttachedDisplay = 0; iAttachedDisplay < d3d.AttachedDisplays.size(); ++iAttachedDisplay) {
		if(d3d.AttachedDisplays[iAttachedDisplay]->Display == displayToAttachTo)
			return ::d3dAttachDisplay(d3d, *d3d.AttachedDisplays[iAttachedDisplay], displayToAttachTo);
	}
	::nwol::ptr_obj<::nwol::SSwapChain>								newSwapchain							= {};
	ree_if(0 == newSwapchain.create(displayToAttachTo), "Out of memory?");
	::nwol::error_t													targetIndex								= d3d.AttachedDisplays.push_back(newSwapchain);
	ree_if(errored(targetIndex), "Failed to push back new display.");
	return ::d3dAttachDisplay(d3d, *d3d.AttachedDisplays[targetIndex], displayToAttachTo);
}
