/// Copyright 2009-2017 - asm128
#include "nwol_input.h"
#include "nwol_bit.h"
#include "fill.h"
#include "nwol_value.h"

#if defined(__WINDOWS__)
#	include <Windows.h>
#endif

static constexpr const char						_pointer_error_string[]						= "Invalid handler pointer: null";
#define check_pointer_error(_hdlr)				reterr_error_if(0 == _hdlr, "%s.", _pointer_error_string)

::nwol::error_t									nwol::registerHandler						(::nwol::SInput& input, IHandlerKeyboard	* handlerKeyboard	)	{ check_pointer_error(handlerKeyboard	);	int32_t pos = input.HandlersKeyboard	.find(handlerKeyboard	); retwarn_warn_if(pos != -1, "Keyboard Handler already registered at index %u."	, pos); const uint32_t expectedIndex = input.HandlersKeyboard	.size(); reterr_error_if(expectedIndex != (uint32_t)input.HandlersKeyboard	.push_back(handlerKeyboard	), "Failed to push pointer to input handler registry. Out of memory?"); return 0; }
::nwol::error_t									nwol::registerHandler						(::nwol::SInput& input, IHandlerMouse		* handlerMouse		)	{ check_pointer_error(handlerMouse		);	int32_t pos = input.HandlersMouse		.find(handlerMouse		); retwarn_warn_if(pos != -1, "Mouse Handler already registered at index %u."		, pos); const uint32_t expectedIndex = input.HandlersMouse		.size(); reterr_error_if(expectedIndex != (uint32_t)input.HandlersMouse		.push_back(handlerMouse		), "Failed to push pointer to input handler registry. Out of memory?"); return 0; }
																																					 	
::nwol::error_t									nwol::unregisterHandler						(::nwol::SInput& input, IHandlerKeyboard	* handlerKeyboard	)	{ check_pointer_error(handlerKeyboard	);	int32_t pos = input.HandlersKeyboard	.find(handlerKeyboard	); retwarn_warn_if(pos == -1, "%s.", "Keyboard handler not registered"	); input.HandlersKeyboard	.remove_unordered(pos);	return 0; }
::nwol::error_t									nwol::unregisterHandler						(::nwol::SInput& input, IHandlerMouse		* handlerMouse		)	{ check_pointer_error(handlerMouse		);	int32_t pos = input.HandlersMouse		.find(handlerMouse		); retwarn_warn_if(pos == -1, "%s.", "Mouse handler not registered"		); input.HandlersMouse		.remove_unordered(pos);	return 0; }

void											handleKeyboardChanges						(const ::nwol::SInput& input)										{
	static constexpr	const uint32_t					keyCount									= ::nwol::SInput::KeyCount;
						uint8_t							countRequiredKeysUp							= 0;
						uint8_t							countRequiredKeysDown						= 0;
						uint8_t							handleRequiredKeysUp		[keyCount]		= {};
						uint8_t							handleRequiredKeysDown		[keyCount]		= {};

	for(uint32_t iKey = 0; iKey < keyCount; ++iKey)	// Store in separate arrays the keys that need to be handled by OnKeyUp() and OnKeyDown(). 
		if( input.Keys[iKey] != input.PreviousKeys[iKey] ) {
			if(input.Keys[iKey] == 0)
				handleRequiredKeysUp	[countRequiredKeysUp	++] = iKey & 0xFF;
			else																
				handleRequiredKeysDown	[countRequiredKeysDown	++] = iKey & 0xFF;
		}

	for(uint32_t iKeyIndex = 0; iKeyIndex < countRequiredKeysUp; ++iKeyIndex) {
#if defined(__WINDOWS__)
		Beep(220+handleRequiredKeysUp[iKeyIndex], 35);
#endif
		for(uint32_t iHandler=0, handlerCount = input.HandlersKeyboard.size(); iHandler<handlerCount; ++iHandler)
			if( input.HandlersKeyboard[iHandler] )
				input.HandlersKeyboard[iHandler]->OnKeyUp(handleRequiredKeysUp[iKeyIndex]);
			else
				error_printf("A null pointer was added to the keyboard handler registry");
	}

	for(uint32_t iKeyIndex = 0; iKeyIndex < countRequiredKeysDown; ++iKeyIndex) {
#if defined(__WINDOWS__)
		Beep(440+handleRequiredKeysUp[iKeyIndex], 35);
#endif
		for(uint32_t iHandler = 0, handlerCount = input.HandlersKeyboard.size(); iHandler < handlerCount; ++iHandler) {
			if( input.HandlersKeyboard[iHandler] )
				input.HandlersKeyboard[iHandler]->OnKeyDown(handleRequiredKeysDown[iKeyIndex]);
			else 
				error_printf("A null pointer was added to the keyboard handler registry");
		}
	}
}

void														handleMouseChanges							(const ::nwol::SInput& input)												{
	static constexpr	const uint32_t								buttonCount									= ::nwol::SInput::ButtonCount;
						uint8_t										countRequiredButtonUp						= 0;
						uint8_t										countRequiredButtonDown						= 0;
						uint8_t										handleRequiredButtonUp		[buttonCount]	= {};
						uint8_t										handleRequiredButtonDown	[buttonCount]	= {};

	for(uint32_t iKey = 0; iKey < buttonCount; ++iKey)	// Store in separate arrays the keys that need to be handled by OnKeyUp() and OnKeyDown(). 
		if( input.Mouse.Buttons[iKey] != input.Mouse.Buttons[iKey] ) {
			if(input.Keys[iKey] == 0)
				handleRequiredButtonUp		[countRequiredButtonUp		++]	= iKey & 0xFF;
			else																
				handleRequiredButtonDown	[countRequiredButtonDown	++]	= iKey & 0xFF;
		}

	for(uint32_t iKeyIndex = 0; iKeyIndex < countRequiredButtonUp; ++iKeyIndex) {
#if defined(__WINDOWS__)
		Beep(220+handleRequiredButtonUp[iKeyIndex]*10, 35);
#endif
		for(uint32_t iHandler=0, handlerCount = input.HandlersKeyboard.size(); iHandler<handlerCount; ++iHandler)
			if( input.HandlersKeyboard[iHandler] )
				input.HandlersKeyboard[iHandler]->OnKeyUp(handleRequiredButtonUp[iKeyIndex]);
			else 
				error_printf("A null pointer to a handler keyboard was added to the keyboard registry");
	}

	for(uint32_t iKeyIndex = 0; iKeyIndex < countRequiredButtonDown; ++iKeyIndex) {
#if defined(__WINDOWS__)
		Beep(440+handleRequiredButtonDown[iKeyIndex]*10, 35);
#endif
		for(uint32_t iHandler=0, handlerCount = input.HandlersKeyboard.size(); iHandler<handlerCount; ++iHandler)
			if( input.HandlersKeyboard[iHandler] )
				input.HandlersKeyboard[iHandler]->OnKeyDown(handleRequiredButtonDown[iKeyIndex]);
			else
				error_printf("A null pointer to a handler keyboard was added to the keyboard registry");
	}
	//------------------------------------------------
}

void														handleInputChanges							(const ::nwol::SInput& input)												{
	::handleKeyboardChanges	(input);
	::handleMouseChanges	(input);
}

::nwol::error_t												createKeyboardDevice						(IDirectInput8* pDirectInput, IDirectInputDevice8** pDirectInputKeyboard)	{
#if defined(__WINDOWS__)
	HRESULT															hr												= 0;
	reterr_error_if(S_OK != (hr = pDirectInput->CreateDevice(GUID_SysKeyboard, pDirectInputKeyboard, NULL))	, "Failed to obtain an interface to the system keyboard device: 0x%X '%s'"	, hr, ::nwol::getOSErrorAsString(hr).c_str());
	reterr_error_if(S_OK != (hr = (*pDirectInputKeyboard)->SetDataFormat(&c_dfDIKeyboard))					, "Failed to set input data format: 0x%X '%s'"								, hr, ::nwol::getOSErrorAsString(hr).c_str());	// Set the data format to "Keyboard format" - a predefined data format. A data format specifies which controls on a device we are interested in, and how they should be reported. This tells DirectInput that we will be passing an array of 256 bytes to IDirectInputDevice::GetDeviceState.
#else
#	error "Not implemented."
#endif
	return 0;
}

::nwol::error_t												createMouseDevice							(IDirectInput8* pDirectInput, IDirectInputDevice8** pDirectInputMouse)		{
#if defined(__WINDOWS__)
	HRESULT															hr												= 0;
	reterr_error_if(S_OK != (hr = pDirectInput->CreateDevice(GUID_SysMouse, pDirectInputMouse, NULL))		, "Failed to obtain an interface to the system keyboard device: 0x%X '%s'"	, hr, ::nwol::getOSErrorAsString(hr).c_str());
	reterr_error_if(S_OK != (hr = (*pDirectInputMouse)->SetDataFormat(&c_dfDIMouse2))						, "Failed to set input data forma: 0x%X '%s'"								, hr, ::nwol::getOSErrorAsString(hr).c_str());	// Set the data format to "Keyboard format" - a predefined data format. A data format specifies which controls on a device we are interested in, and how they should be reported. This tells DirectInput that we will be passing an array of 256 bytes to IDirectInputDevice::GetDeviceState.
#else
#	error "Not implemented."
#endif
	return 0;
}

//struct auto_com_release : public ::nwol::platform_handle_wrapper<IUnknown*, 0>					{ using TWrapper::platform_handle_wrapper; inline ~auto_com_release() { close(); } inline void close() { safe_com_release(Handle); } };
::nwol::error_t												inputInitialize									(::nwol::SScreenInput& input, ::nwol::SScreenDetail& screenDetail)														{
	::nwol::SInputDetail											& inputDetail									= input.PlatformDetail;
#if defined(__WINDOWS__)
	HRESULT															hr												= 0;
	if(0 == inputDetail.DirectInputContext) {
		reterr_error_if(S_OK != (hr = DirectInput8Create(screenDetail.pWindowClass->hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, ( void** )&inputDetail.DirectInputContext, NULL)), "Cannot create DirectInput context: 0x%X '%s'", hr, ::nwol::getWindowsErrorAsString(hr).c_str())
		else info_printf("IDirectInput8 created successfully.");
	}
	if(0 == inputDetail.DirectInputKeyboard) {
		IDirectInputDevice8												* newDIDevice								= nullptr;
		error_if(createKeyboardDevice	(inputDetail.DirectInputContext, &newDIDevice), "Failed to create IDirectInputDevice8 for keyboard polling.")
		else {
			inputDetail.DirectInputKeyboard								= newDIDevice;
			info_printf("Keyboard device created.");
			gbit_set(inputDetail.DeviceFlagsKeyboard	, ::nwol::WINDOWS_INPUT_STATE_FLAG_Initialized);
		}
	}
	if(0 == input.PlatformDetail.DirectInputMouse) {
		IDirectInputDevice8												* newDIDevice								= nullptr;
		error_if(createMouseDevice		(input.PlatformDetail.DirectInputContext, &newDIDevice), "Failed to create IDirectInputDevice8 for mouse polling."	)
		else {
			inputDetail.DirectInputMouse								= newDIDevice;
			info_printf("Mouse device created.");
			gbit_set(input.PlatformDetail.DeviceFlagsMouse		, ::nwol::WINDOWS_INPUT_STATE_FLAG_Initialized);
		}
	}
#else
#	error "Not implemented."
#endif
	return 0;
}

::nwol::error_t												nwol::setCooperativeLevels						(::nwol::SScreenDetail& screenDetail, ::nwol::SScreenInput& input)		{
	::nwol::SInputDetail											& inputDetail									= input.PlatformDetail;
	HRESULT															hr												= 0;
	::nwol::error_t													finalError										= 0;
	reterr_error_if(0 == screenDetail.hWnd, "Cannot set cooperative level without a window to cooperate with.");

	if (gbit_false(input.PlatformDetail.DeviceFlagsKeyboard, ::nwol::WINDOWS_INPUT_STATE_FLAG_Initialized) || gbit_false(input.PlatformDetail.DeviceFlagsMouse, ::nwol::WINDOWS_INPUT_STATE_FLAG_Initialized))
		nwol_necall(inputInitialize(input, screenDetail), "Failed to initialize low level input system.");

	{	// ----------------- Set up keyboard cooperative level
		DWORD															dwCoopFlags										= gbit_true(inputDetail.DeviceFlagsKeyboard, ::nwol::WINDOWS_INPUT_STATE_FLAG_Exclusive) ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE;
		if gbit_false(inputDetail.DeviceFlagsKeyboard, ::nwol::WINDOWS_INPUT_STATE_FLAG_Background)
			gbit_set(dwCoopFlags, DISCL_FOREGROUND);
		else if gbit_true(inputDetail.DeviceFlagsKeyboard, ::nwol::WINDOWS_INPUT_STATE_FLAG_Exclusive) {
			gbit_clear(inputDetail.DeviceFlagsKeyboard, ::nwol::WINDOWS_INPUT_STATE_FLAG_Background);
			gbit_set(dwCoopFlags, DISCL_FOREGROUND);
			warning_printf("For security reasons, background exclusive mouse access is not allowed. Setting to foreground mode");
		}
		else
			gbit_set(dwCoopFlags, DISCL_BACKGROUND);

		error_if((hr = inputDetail.DirectInputKeyboard->SetCooperativeLevel( screenDetail.hWnd, dwCoopFlags )) == DIERR_UNSUPPORTED && gbit_true(inputDetail.DeviceFlagsKeyboard, ::nwol::WINDOWS_INPUT_STATE_FLAG_Background) && gbit_true(inputDetail.DeviceFlagsKeyboard, ::nwol::WINDOWS_INPUT_STATE_FLAG_Exclusive), "SetCooperativeLevel() returned DIERR_UNSUPPORTED. For security reasons, background exclusive keyboard access is not allowed.");
		if(hr)
			finalError													= -1;
		else
			info_printf("Cooperative mode set for keyboard: 0x%X", dwCoopFlags);

	}
	{	// ----------------- Set up mouse cooperative level
		DWORD															dwCoopFlags										= gbit_true(inputDetail.DeviceFlagsMouse, ::nwol::WINDOWS_INPUT_STATE_FLAG_Exclusive) ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE;
		if gbit_false(inputDetail.DeviceFlagsMouse, ::nwol::WINDOWS_INPUT_STATE_FLAG_Background)
			gbit_set(dwCoopFlags, DISCL_FOREGROUND);
		else if gbit_true(inputDetail.DeviceFlagsMouse, ::nwol::WINDOWS_INPUT_STATE_FLAG_Exclusive) {
			gbit_clear(inputDetail.DeviceFlagsMouse, ::nwol::WINDOWS_INPUT_STATE_FLAG_Background);
			gbit_set(dwCoopFlags, DISCL_FOREGROUND);
			warning_printf("For security reasons, background exclusive mouse access is not allowed. Setting to foreground mode");
		}
		else
			gbit_set(dwCoopFlags, DISCL_BACKGROUND);
		reterr_error_if((hr = inputDetail.DirectInputMouse->SetCooperativeLevel( screenDetail.hWnd, dwCoopFlags )) == DIERR_UNSUPPORTED && gbit_true(inputDetail.DeviceFlagsMouse, ::nwol::WINDOWS_INPUT_STATE_FLAG_Background) && gbit_true(inputDetail.DeviceFlagsMouse, ::nwol::WINDOWS_INPUT_STATE_FLAG_Exclusive), "SetCooperativeLevel() returned DIERR_UNSUPPORTED. For security reasons, background exclusive keyboard access is not allowed.");
		info_printf("Cooperative mode set for mouse: 0x%X", dwCoopFlags);
	}
	return 0;
}

::nwol::error_t												nwol::pollInput									(SInput& input)															{
	static constexpr	const uint32_t								keyCount										= ::nwol::SInput::KeyCount;
	static constexpr	const uint32_t								buttonCount										= ::nwol::SInput::ButtonCount;

	//if (gbit_false(input.PlatformDetail.DeviceFlagsKeyboard, ::nwol::WINDOWS_INPUT_STATE_FLAG_Initialized) || gbit_false(input.PlatformDetail.DeviceFlagsMouse, ::nwol::WINDOWS_INPUT_STATE_FLAG_Initialized))
	//	error_if(errored(inputInitialize(input)), "Failed to initialize low level input system.");

	memcpy(input.PreviousKeys, input.Keys, keyCount);
#if defined(__WINDOWS__)
	for(uint32_t i=0; i<keyCount; i++)
		input.Keys[i]												= GetAsyncKeyState(i) ? 1 : 0;
#else
#	error "Not implemented."
#endif
	memcpy(input.PreviousMouse.Buttons, input.Mouse.Buttons, buttonCount);

#if defined(__WINDOWS__)
						INPUT_RECORD								recordIn										= {};
						DWORD										NumRead											= 0;
						HANDLE										handleIn										= GetStdHandle(STD_INPUT_HANDLE);
						DWORD										eventCount										= 0;

	GetNumberOfConsoleInputEvents (handleIn, &eventCount);
	if( 0 == eventCount )
		return 0;

	ReadConsoleInput(handleIn, &recordIn, 1, &NumRead);

	switch (recordIn.EventType) {
	case KEY_EVENT:
		// Do stuff
		//input.Keys[recordIn.Event.KeyEvent.wVirtualKeyCode] = recordIn.Event.KeyEvent.bKeyDown ? 1 : 0;
		break;
	case MOUSE_EVENT:
		input.Mouse.Deltas.x										= recordIn.Event.MouseEvent.dwMousePosition.X;
		input.Mouse.Deltas.y										= recordIn.Event.MouseEvent.dwMousePosition.Y;
		input.Mouse.Buttons[0]										= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED	;
		input.Mouse.Buttons[1]										= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED	;
		input.Mouse.Buttons[2]										= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_3RD_BUTTON_PRESSED	;
		input.Mouse.Buttons[3]										= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_4TH_BUTTON_PRESSED	;
		input.Mouse.Buttons[4]										= recordIn.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED		;
		break;
	}
#else
#	error "Not implemented."
#endif
	::handleInputChanges(input);
	return 0;
}

				::nwol::error_t								nwol::unacquireInput							(::nwol::SScreenInput& input)											{
	::nwol::SInputDetail											& inputDetail									= input.PlatformDetail;
	HRESULT															hr;

	error_if(0 == inputDetail.DirectInputKeyboard, "No DirectInput device for keyboard input.")
	else if gbit_true(inputDetail.DeviceFlagsKeyboard, ::nwol::WINDOWS_INPUT_STATE_FLAG_Acquired) {
		error_if(DI_OK != (hr = inputDetail.DirectInputKeyboard->Unacquire()), "Failed to unacquire keyboard: 0x%X '%s'", hr, ::nwol::getWindowsErrorAsString(hr).c_str())
		else {
			info_printf("Unacquired keyboard device.");
			gbit_clear(inputDetail.DeviceFlagsKeyboard	, ::nwol::WINDOWS_INPUT_STATE_FLAG_Acquired);
		}
		::nwol::array_copy(input.PreviousKeys, input.Keys);
		::nwol::fillArray(input.Keys, (uint8_t)0U);
	}
	else
		warning_printf("DirectInput device is already unacquired for keyboard input.");

	error_if(0 == inputDetail.DirectInputMouse, "No DirectInput device for mouse input.")
	else if(gbit_true(inputDetail.DeviceFlagsMouse	, ::nwol::WINDOWS_INPUT_STATE_FLAG_Acquired)) {
		error_if(DI_OK != (hr = inputDetail.DirectInputMouse	->Unacquire()), "Failed to unacquire mouse: 0x%X '%s'", hr, ::nwol::getWindowsErrorAsString(hr).c_str())
		else {
			info_printf("Unacquired mouse device.");
			gbit_clear(inputDetail.DeviceFlagsKeyboard	, ::nwol::WINDOWS_INPUT_STATE_FLAG_Acquired);
		}
		input.PreviousMouse											= input.Mouse;
		input.Mouse													= {};
	}
	else
		warning_printf("DirectInput device is already unacquired for mouse input.");
	return 0;
}
				::nwol::error_t								nwol::acquireInput								(::nwol::SScreenInput& input)											{
	::nwol::SInputDetail											& inputDetail									= input.PlatformDetail;
	HRESULT															hr;
	error_if(0 == inputDetail.DirectInputKeyboard, "No DirectInput device for keyboard input.")
	else if(gbit_false(inputDetail.DeviceFlagsKeyboard, ::nwol::WINDOWS_INPUT_STATE_FLAG_Acquired)) {
		info_if(DI_OK != (hr = inputDetail.DirectInputKeyboard	->Acquire()), "Failed to acquire keyboard: 0x%X '%s'.", hr, ::nwol::getWindowsErrorAsString(hr).c_str())
		else {
			info_printf("Acquired keyboard device.");
			gbit_set(inputDetail.DeviceFlagsKeyboard	, ::nwol::WINDOWS_INPUT_STATE_FLAG_Acquired);
		}
		::nwol::array_copy(input.PreviousKeys, input.Keys);
		::nwol::fillArray(input.Keys, (uint8_t)0U);
	}
	else
		warning_printf("DirectInput device is already acquired for keyboard input.");

	error_if(0 == inputDetail.DirectInputMouse, "No DirectInput device for mouse input.")
	else if(gbit_false(inputDetail.DeviceFlagsMouse	, ::nwol::WINDOWS_INPUT_STATE_FLAG_Acquired)) {
		info_if(DI_OK != (hr = inputDetail.DirectInputMouse		->Acquire()), "Failed to acquire mouse: 0x%X '%s'.", hr, ::nwol::getWindowsErrorAsString(hr).c_str())
		else {
			info_printf("Acquired mouse device.");
			gbit_set(inputDetail.DeviceFlagsMouse		, ::nwol::WINDOWS_INPUT_STATE_FLAG_Acquired);
		}
		input.PreviousMouse											= input.Mouse;
		input.Mouse													= {};
	}
	else
		warning_printf("DirectInput device is already acquired for mouse input.");
	return 0;
}

::nwol::error_t												nwol::pollInput									(::nwol::SScreenInput& input, ::nwol::SScreenDetail boundScreen)		{
	static constexpr	const uint32_t								keyCount										= ::nwol::SInput::KeyCount;
	static constexpr	const uint32_t								buttonCount										= ::nwol::SInput::ButtonCount;

	if (gbit_false(input.PlatformDetail.DeviceFlagsKeyboard, ::nwol::WINDOWS_INPUT_STATE_FLAG_Initialized) || gbit_false(input.PlatformDetail.DeviceFlagsMouse, ::nwol::WINDOWS_INPUT_STATE_FLAG_Initialized)) 
		error_if(errored(inputInitialize(input, boundScreen)), "Failed to initialize low level input system.");

	for(uint32_t iMessage = 0, messageCount = boundScreen.Messages.size(); iMessage < messageCount; ++iMessage) 
		switch(boundScreen.Messages[iMessage].uMsg) {
		case WM_ACTIVATE:
			if( WA_INACTIVE != boundScreen.Messages[iMessage].wParam ) {
				if gbit_true(input.PlatformDetail.DeviceFlagsKeyboard, ::nwol::WINDOWS_INPUT_STATE_FLAG_Initialized)  { if(0 == input.PlatformDetail.DirectInputKeyboard	->Acquire())	{ gbit_set(input.PlatformDetail.DeviceFlagsKeyboard		, ::nwol::WINDOWS_INPUT_STATE_FLAG_Acquired); info_printf("Acquired input device."	); } else warning_printf("Failed to acquire device");	}
				if gbit_true(input.PlatformDetail.DeviceFlagsMouse	 , ::nwol::WINDOWS_INPUT_STATE_FLAG_Initialized)  { if(0 == input.PlatformDetail.DirectInputMouse		->Acquire())	{ gbit_set(input.PlatformDetail.DeviceFlagsMouse		, ::nwol::WINDOWS_INPUT_STATE_FLAG_Acquired); info_printf("Acquired input device."	); } else warning_printf("Failed to acquire device");	}
			}		 																								
			else { 																									
				if gbit_true(input.PlatformDetail.DeviceFlagsKeyboard, ::nwol::WINDOWS_INPUT_STATE_FLAG_Initialized)  { if(0 == input.PlatformDetail.DirectInputKeyboard	->Unacquire())	{ gbit_clear(input.PlatformDetail.DeviceFlagsKeyboard	, ::nwol::WINDOWS_INPUT_STATE_FLAG_Acquired); info_printf("Unacquired input device."); } else warning_printf("Failed to unacquire device");	}
				if gbit_true(input.PlatformDetail.DeviceFlagsMouse	 , ::nwol::WINDOWS_INPUT_STATE_FLAG_Initialized)  { if(0 == input.PlatformDetail.DirectInputMouse		->Unacquire())	{ gbit_clear(input.PlatformDetail.DeviceFlagsMouse		, ::nwol::WINDOWS_INPUT_STATE_FLAG_Acquired); info_printf("Unacquired input device."); } else warning_printf("Failed to unacquire device");	}
			}
		}


	memcpy(input.PreviousKeys, input.Keys, keyCount);
#if defined(__WINDOWS__)
	for(uint32_t i=0; i<keyCount; i++)
		input.Keys[i]												= GetAsyncKeyState(i) ? 1 : 0;
#else
#	error "Not implemented."
#endif
	memcpy(input.PreviousMouse.Buttons, input.Mouse.Buttons, buttonCount);

#if defined(__WINDOWS__)
						INPUT_RECORD								recordIn										= {};
						DWORD										NumRead											= 0;
						HANDLE										handleIn										= GetStdHandle(STD_INPUT_HANDLE);
						DWORD										eventCount										= 0;

	GetNumberOfConsoleInputEvents (handleIn, &eventCount);
	if( 0 == eventCount )
		return 0;

	ReadConsoleInput(handleIn, &recordIn, 1, &NumRead);

	switch (recordIn.EventType) {
	case KEY_EVENT:
		// Do stuff
		//input.Keys[recordIn.Event.KeyEvent.wVirtualKeyCode] = recordIn.Event.KeyEvent.bKeyDown ? 1 : 0;
		break;
	case MOUSE_EVENT:
		input.Mouse.Deltas.x										= recordIn.Event.MouseEvent.dwMousePosition.X;
		input.Mouse.Deltas.y										= recordIn.Event.MouseEvent.dwMousePosition.Y;
		input.Mouse.Buttons[0]										= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED	;
		input.Mouse.Buttons[1]										= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED	;
		input.Mouse.Buttons[2]										= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_3RD_BUTTON_PRESSED	;
		input.Mouse.Buttons[3]										= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_4TH_BUTTON_PRESSED	;
		input.Mouse.Buttons[4]										= recordIn.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED		;
		break;
	}
#else
#	error "Not implemented."
#endif
	::handleInputChanges(input);
	return 0;
}
