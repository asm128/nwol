#include "input.h"
#include "nwol_error.h"

#if defined(__WINDOWS__)
#	include <Windows.h>
#endif

static const char								_pointer_error_string[]						= "Invalid handler pointer: null";
#define check_pointer_error(_hdlr)	reterr_error_if(0 == _hdlr, "%s.", _pointer_error_string)

::nwol::error_t		nwol::registerHandler	(::nwol::SInput& input, IHandlerKeyboard	* handler)	{ check_pointer_error(handler);	int32_t pos = input.HandlersKeyboard	.find(handler); retwarn_msg_if(pos != -1, "Keyboard Handler already registered at index %u.", pos); const uint32_t expectedIndex = input.HandlersKeyboard	.size(); reterr_error_if(expectedIndex != (uint32_t)input.HandlersKeyboard	.push_back(handler), "Failed to push pointer to input handler registry. Out of memory?"); return 0; }
::nwol::error_t		nwol::registerHandler	(::nwol::SInput& input, IHandlerMouse		* handler)	{ check_pointer_error(handler);	int32_t pos = input.HandlersMouse		.find(handler); retwarn_msg_if(pos != -1, "Mouse Handler already registered at index %u."	, pos); const uint32_t expectedIndex = input.HandlersMouse		.size(); reterr_error_if(expectedIndex != (uint32_t)input.HandlersMouse		.push_back(handler), "Failed to push pointer to input handler registry. Out of memory?"); return 0; }
																									 	
::nwol::error_t		nwol::unregisterHandler	(::nwol::SInput& input, IHandlerKeyboard	* handler)	{ check_pointer_error(handler);	int32_t pos = input.HandlersKeyboard	.find(handler); retwarn_msg_if(pos == -1, "%s.", "Keyboard handler not registered"	); input.HandlersKeyboard	.remove_unordered(pos);	return 0; }
::nwol::error_t		nwol::unregisterHandler	(::nwol::SInput& input, IHandlerMouse		* handler)	{ check_pointer_error(handler);	int32_t pos = input.HandlersMouse		.find(handler); retwarn_msg_if(pos == -1, "%s.", "Mouse handler not registered"		); input.HandlersMouse		.remove_unordered(pos);	return 0; }

void											handleKeyboardChanges						(const ::nwol::SInput& input)			{

	static constexpr	const uint32_t					keyCount									= ::nwol::SInput::KeyCount;
						uint8_t							handleCountRequiredKeyUp					= 0;
						uint8_t							handleCountRequiredKeyDown					= 0;
						uint8_t							handleRequiredKeyUp			[keyCount]		= {};
						uint8_t							handleRequiredKeyDown		[keyCount]		= {};

	for(uint32_t iKey=0; iKey<keyCount; ++iKey)
		if( input.Keys[iKey] != input.PreviousKeys[iKey] ) {
			if(input.Keys[iKey] == 0)
				handleRequiredKeyUp		[handleCountRequiredKeyUp++]	= iKey & 0xFF;
			else																
				handleRequiredKeyDown	[handleCountRequiredKeyDown++]	= iKey & 0xFF;
		}

	for(uint32_t iKeyIndex=0; iKeyIndex<handleCountRequiredKeyUp; ++iKeyIndex) {
#if defined(__WINDOWS__)
		Beep(220+handleRequiredKeyUp[iKeyIndex], 35);
#endif
		for(uint32_t iHandler=0, handlerCount = input.HandlersKeyboard.size(); iHandler<handlerCount; ++iHandler)
			if(0 != input.HandlersKeyboard[iHandler])
				input.HandlersKeyboard[iHandler]->OnKeyUp(handleRequiredKeyUp[iKeyIndex]);
			else {
				error_printf("A null pointer to a handler keyboard was added to the keyboar registry");
				continue;
			}
	}

	for(uint32_t iKeyIndex=0; iKeyIndex<handleCountRequiredKeyDown; ++iKeyIndex) {
#if defined(__WINDOWS__)
		Beep(440+handleRequiredKeyUp[iKeyIndex], 35);
#endif
		for(uint32_t iHandler=0, handlerCount = input.HandlersKeyboard.size(); iHandler<handlerCount; ++iHandler) {
			if(0 != input.HandlersKeyboard[iHandler])
				input.HandlersKeyboard[iHandler]->OnKeyDown(handleRequiredKeyDown[iKeyIndex]);
			else {
				error_printf("A null pointer to a handler keyboard was added to the keyboar registry");
				continue;
			}
		}
	}
}

void											handleMouseChanges							(const ::nwol::SInput& input)			{
	//------------------------------------------------
	static constexpr	const uint32_t					buttonCount									= ::nwol::SInput::ButtonCount;

						uint8_t							handleCountRequiredButtonUp					= 0;
						uint8_t							handleCountRequiredButtonDown				= 0;
						uint8_t							handleRequiredButtonUp		[buttonCount]	= {};
						uint8_t							handleRequiredButtonDown	[buttonCount]	= {};

	for(uint32_t iKey=0; iKey<buttonCount; ++iKey)
		if( input.MouseButtons[iKey] != input.MouseButtons[iKey] ) {
			if(input.Keys[iKey] == 0)
				handleRequiredButtonUp		[handleCountRequiredButtonUp	++]	= iKey & 0xFF;
			else																
				handleRequiredButtonDown	[handleCountRequiredButtonDown	++]	= iKey & 0xFF;
		}

	for(uint32_t iKeyIndex=0; iKeyIndex<handleCountRequiredButtonUp; ++iKeyIndex) {
#if defined(__WINDOWS__)
		Beep(220+handleRequiredButtonUp[iKeyIndex], 35);
#endif
		for(uint32_t iHandler=0, handlerCount = input.HandlersKeyboard.size(); iHandler<handlerCount; ++iHandler)
			if(0 != input.HandlersKeyboard[iHandler])
				input.HandlersKeyboard[iHandler]->OnKeyUp(handleRequiredButtonUp[iKeyIndex]);
			else {
				error_printf("A null pointer to a handler keyboard was added to the keyboar registry");
				continue;
			}
	}

	for(uint32_t iKeyIndex=0; iKeyIndex<handleCountRequiredButtonDown; ++iKeyIndex) {
#if defined(__WINDOWS__)
		Beep(440+handleRequiredButtonDown[iKeyIndex], 35);
#endif
		for(uint32_t iHandler=0, handlerCount = input.HandlersKeyboard.size(); iHandler<handlerCount; ++iHandler)
			if(0 != input.HandlersKeyboard[iHandler])
				input.HandlersKeyboard[iHandler]->OnKeyDown(handleRequiredButtonDown[iKeyIndex]);
			else {
				error_printf("A null pointer to a handler keyboard was added to the keyboar registry");
				continue;
			}
	}
	//------------------------------------------------
}

void											handleInputChanges							(const ::nwol::SInput& input)			{
	handleKeyboardChanges	(input);
	handleMouseChanges		(input);
}

void											nwol::pollInput								(SInput& input)							{
	static constexpr	const uint32_t						keyCount								= ::nwol::SInput::KeyCount;
	static constexpr	const uint32_t						buttonCount								= ::nwol::SInput::ButtonCount;

	memcpy(input.PreviousKeys, input.Keys, keyCount);
#if defined(__WINDOWS__)
	for(uint32_t i=0; i<keyCount; i++)
		input.Keys[i]										= GetAsyncKeyState(i) ? 1 : 0;
#else
#error "Not implemented."
#endif

	memcpy(input.PreviousMouseButtons, input.MouseButtons, buttonCount);

#if defined(__WINDOWS__)
						INPUT_RECORD						recordIn								= {};
						DWORD								NumRead									= 0;
						HANDLE								handleIn								= GetStdHandle(STD_INPUT_HANDLE);
						DWORD								eventCount								= 0;

	GetNumberOfConsoleInputEvents (handleIn, &eventCount);
	if( 0 == eventCount )
		return;

	ReadConsoleInput(handleIn, &recordIn, 1, &NumRead);

	switch (recordIn.EventType) {
	case KEY_EVENT:
		// Do stuff
		//input.Keys[recordIn.Event.KeyEvent.wVirtualKeyCode] = recordIn.Event.KeyEvent.bKeyDown ? 1 : 0;
		break;
	case MOUSE_EVENT:
		input.MouseX										= recordIn.Event.MouseEvent.dwMousePosition.X;
		input.MouseY										= recordIn.Event.MouseEvent.dwMousePosition.Y;
		input.MouseButtons[0]								= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED	;
		input.MouseButtons[1]								= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED	;
		input.MouseButtons[2]								= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_3RD_BUTTON_PRESSED	;
		input.MouseButtons[3]								= recordIn.Event.MouseEvent.dwButtonState & FROM_LEFT_4TH_BUTTON_PRESSED	;
		input.MouseButtons[4]								= recordIn.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED		;
		break;
	}
#else
#error "Not implemented."
#endif

	handleInputChanges(input);
}

