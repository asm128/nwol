/// Copyright 2016-2017 - asm128
#include "nwol_enum.h"
#include "nwol_screen.h"

#if defined(__WINDOWS__)
#	define DIRECTINPUT_VERSION 0x0800
#	include <dinput.h>
#endif

#ifndef __INPUT_H__9263487236498723649213640918273098__
#define __INPUT_H__9263487236498723649213640918273098__

namespace nwol
{
	struct IHandlerKeyboard		{
		virtual				void										OnKeyUp									(uint8_t key)												= 0;
		virtual				void										OnKeyDown								(uint8_t key)												= 0;
	};
	struct IHandlerMouse		{
		virtual				void										OnMouseButtonUp							(uint8_t buttonIndex)										= 0;
		virtual				void										OnMouseButtonDown						(uint8_t buttonIndex)										= 0;
		virtual				void										OnMouseMove								(int32_t x, int32_t y, int32_t z)							= 0;
	};
	GDEFINE_FLAG_TYPE(WINDOWS_INPUT_STATE_FLAG, int8_t);
	GDEFINE_FLAG_VALUE(WINDOWS_INPUT_STATE_FLAG,	Default				, 0x00);
	GDEFINE_FLAG_VALUE(WINDOWS_INPUT_STATE_FLAG,	Initialized			, 0x01);
	GDEFINE_FLAG_VALUE(WINDOWS_INPUT_STATE_FLAG,	Buffered			, 0x02);
	GDEFINE_FLAG_VALUE(WINDOWS_INPUT_STATE_FLAG,	Exclusive			, 0x04);
	GDEFINE_FLAG_VALUE(WINDOWS_INPUT_STATE_FLAG,	Background			, 0x08);
	GDEFINE_FLAG_VALUE(WINDOWS_INPUT_STATE_FLAG,	DisableWindowsKey	, 0x10);
	GDEFINE_FLAG_VALUE(WINDOWS_INPUT_STATE_FLAG,	Acquired			, 0x20);

	struct SMouseInput {
							::nwol::SCoord3<int32_t>					Deltas									;
							uint8_t										Buttons					[16]			;
	};

	struct SInput				{
		static constexpr	const uint16_t								KeyCount								= 256U;
		static constexpr	const uint16_t								ButtonCount								= 16U;

							::nwol::array_pod<IHandlerKeyboard	*>		HandlersKeyboard						= {};
							::nwol::array_pod<IHandlerMouse		*>		HandlersMouse							= {};

							SMouseInput									Mouse									;
							SMouseInput									PreviousMouse							;

							uint8_t										Keys					[KeyCount]		= {};
							uint8_t										PreviousKeys			[KeyCount]		= {};

		inline				bool										KeyUp									(uint8_t index)							const	noexcept	{ return 0 == Keys			[index] && 0 != PreviousKeys			[index]; }
		inline				bool										KeyDown									(uint8_t index)							const	noexcept	{ return 0 != Keys			[index] && 0 == PreviousKeys			[index]; }
		inline				bool										ButtonUp								(uint8_t index)							const	noexcept	{ return 0 == Mouse.Buttons	[index] && 0 != PreviousMouse.Buttons	[index]; }
		inline				bool										ButtonDown								(uint8_t index)							const	noexcept	{ return 0 != Mouse.Buttons	[index] && 0 == PreviousMouse.Buttons	[index]; }
	};

		struct SInputDetail	{
	#if defined(__ANDROID__)
								void										(*handleAppInput)						()														= nullptr;
	#elif defined(__WINDOWS__)
								WINDOWS_INPUT_STATE_FLAG					DeviceFlagsKeyboard						= WINDOWS_INPUT_STATE_FLAG_Default;
								WINDOWS_INPUT_STATE_FLAG					DeviceFlagsMouse						= WINDOWS_INPUT_STATE_FLAG_Default;
								IDirectInput8								* DirectInputContext					= nullptr;	
								IDirectInputDevice8							* DirectInputKeyboard					= nullptr;	// For now we just support the basics and further we should move to xinput or similar.
								IDirectInputDevice8							* DirectInputMouse						= nullptr;	
								::nwol::array_pod<::nwol::SScreen>			WindowList;
	#else
								void										(*handleAppInput)						()														= nullptr;
	#endif

																			~SInputDetail							()														{ 
	#if defined(__WINDOWS__)
				if(DirectInputMouse		&& gbit_true(DeviceFlagsMouse	, WINDOWS_INPUT_STATE_FLAG_Acquired)) DirectInputMouse		->Unacquire();
				if(DirectInputKeyboard	&& gbit_true(DeviceFlagsKeyboard, WINDOWS_INPUT_STATE_FLAG_Acquired)) DirectInputKeyboard	->Unacquire();
				safe_com_release(DirectInputMouse	); 
				safe_com_release(DirectInputKeyboard); 
				safe_com_release(DirectInputContext	); 
				info_printf("DirectInput interfaces released.");
	#endif
			}
		};
		
	struct SScreenInput	: public ::nwol::SInput {
		using				::nwol::SInput::							KeyCount								;
		using				::nwol::SInput::							ButtonCount								;
		using				::nwol::SInput::							HandlersKeyboard						;
		using				::nwol::SInput::							HandlersMouse							;
		using				::nwol::SInput::							Mouse									;
		using				::nwol::SInput::							PreviousMouse							;
		using				::nwol::SInput::							Keys									;
		using				::nwol::SInput::							PreviousKeys							;

							::nwol::SInputDetail						PlatformDetail;

		inline				bool										KeyUp									(uint8_t index)							const	noexcept	{ return 0 == Keys			[index] && 0 != PreviousKeys			[index]; }
		inline				bool										KeyDown									(uint8_t index)							const	noexcept	{ return 0 != Keys			[index] && 0 == PreviousKeys			[index]; }
		inline				bool										ButtonUp								(uint8_t index)							const	noexcept	{ return 0 == Mouse.Buttons	[index] && 0 != PreviousMouse.Buttons	[index]; }
		inline				bool										ButtonDown								(uint8_t index)							const	noexcept	{ return 0 != Mouse.Buttons	[index] && 0 == PreviousMouse.Buttons	[index]; }
	};

	enum INPUT_EVENT : int8_t 
		{	INPUT_EVENT_IDLE					= 0
		,	INPUT_EVENT_KEY_DOWN				= 1
		,	INPUT_EVENT_KEY_UP					= 2
		,	INPUT_EVENT_MOUSE_MOVE				= 3
		,	INPUT_EVENT_MOUSE_BUTTON_DOWN		= 4
		,	INPUT_EVENT_MOUSE_BUTTON_UP			= 5
		,	INPUT_EVENT_GAMEPAD_BUTTON_DOWN		= 6
		,	INPUT_EVENT_GAMEPAD_BUTTON_UP		= 7
		,	INPUT_EVENT_INVALID					= -1
		};
						::nwol::error_t								pollInput								(::nwol::SInput& input);

						// registerHandler functions return 0 on success, 1 if the handler was already registered or -1 on fatal error (such as registering a null handler or being unable to register handler because of lack of available memory) .
						::nwol::error_t								registerHandler							(::nwol::SInput& input, ::nwol::IHandlerKeyboard	* handler);
						::nwol::error_t								registerHandler							(::nwol::SInput& input, ::nwol::IHandlerMouse		* handler);

						// unregisterHandler functions return 0 on success, 1 if the handler was not registered or -1 if the handler pointer is null.
						::nwol::error_t								unregisterHandler						(::nwol::SInput& input, ::nwol::IHandlerKeyboard	* handler);
						::nwol::error_t								unregisterHandler						(::nwol::SInput& input, ::nwol::IHandlerMouse		* handler);

						::nwol::error_t								pollInput								(::nwol::SScreenInput& input		, ::nwol::SScreenDetail boundScreen);
						::nwol::error_t								setCooperativeLevels					(::nwol::SScreenDetail& screenDetail, ::nwol::SScreenInput& input);
						::nwol::error_t								unacquireInput							(::nwol::SScreenInput& input);
						::nwol::error_t								acquireInput							(::nwol::SScreenInput& input);
} // namespace
#endif // __INPUT_H__9263487236498723649213640918273098__
