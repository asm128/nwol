/// Copyright 2016-2017 - asm128
#include "nwol_enum.h"

#ifndef __INPUT_H__9263487236498723649213640918273098__
#define __INPUT_H__9263487236498723649213640918273098__

namespace nwol
{
	struct IHandlerKeyboard		{
		virtual				void										OnKeyUp									(uint8_t key)											= 0;
		virtual				void										OnKeyDown								(uint8_t key)											= 0;
	};
	struct IHandlerMouse		{
		virtual				void										OnMouseButtonUp							(uint8_t buttonIndex)									= 0;
		virtual				void										OnMouseButtonDown						(uint8_t buttonIndex)									= 0;
		virtual				void										OnMouseMove								(int32_t x, int32_t y, int32_t z)						= 0;
	};
	GDEFINE_FLAG_TYPE(WINDOWS_INPUT_DEVICE_FLAG, uint8_t);
	GDEFINE_FLAG_VALUE(WINDOWS_INPUT_DEVICE_FLAG,					Default				, 0x00);
	GDEFINE_FLAG_VALUE(WINDOWS_INPUT_DEVICE_FLAG,					Buffered			, 0x01);
	GDEFINE_FLAG_VALUE(WINDOWS_INPUT_DEVICE_FLAG,					Exclusive			, 0x02);
	GDEFINE_FLAG_VALUE(WINDOWS_INPUT_DEVICE_FLAG,					Foreground			, 0x04);
	GDEFINE_FLAG_VALUE(WINDOWS_INPUT_DEVICE_FLAG,					DisableWindowsKey	, 0x08);
	GDEFINE_FLAG_VALUE(WINDOWS_INPUT_DEVICE_FLAG,					Acquired			, 0x10);

	struct SInput				{
		static constexpr	const uint16_t								KeyCount								= 256U;
		static constexpr	const uint16_t								ButtonCount								= 16U;

							::nwol::array_pod<IHandlerKeyboard	*>		HandlersKeyboard						= {};
							::nwol::array_pod<IHandlerMouse		*>		HandlersMouse							= {};

							int32_t										MouseX									= 0;
							int32_t										MouseY									= 0;

							uint8_t										Keys					[KeyCount]		= {};
							uint8_t										PreviousKeys			[KeyCount]		= {};
							uint8_t										MouseButtons			[ButtonCount]	= {};
							uint8_t										PreviousMouseButtons	[ButtonCount]	= {};
		struct SInputDetail			{
#if defined(__ANDROID__)
								void										(*handleAppInput)						()														= nullptr;
#elif defined(__WINDOWS__)
								WINDOWS_INPUT_DEVICE_FLAG					DeviceFlags								= WINDOWS_INPUT_DEVICE_FLAG_Default;
#else
								void										(*handleAppInput)						()														= nullptr;
#endif
		}																PlatformDetail;

		inline				bool										KeyUp									(uint8_t index)						const	noexcept	{ return 0 == Keys			[index] && 0 != PreviousKeys			[index]; }
		inline				bool										KeyDown									(uint8_t index)						const	noexcept	{ return 0 != Keys			[index] && 0 == PreviousKeys			[index]; }
		inline				bool										ButtonUp								(uint8_t index)						const	noexcept	{ return 0 == MouseButtons	[index] && 0 != PreviousMouseButtons	[index]; }
		inline				bool										ButtonDown								(uint8_t index)						const	noexcept	{ return 0 != MouseButtons	[index] && 0 == PreviousMouseButtons	[index]; }
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
						::nwol::error_t								inputInitialize							(SInput& input);
						::nwol::error_t								pollInput								(SInput& input);

						// registerHandler functions return 0 on success, 1 if the handler was already registered or -1 on fatal error (such as registering a null handler or being unable to register handler because of lack of available memory) .
						::nwol::error_t								registerHandler							(SInput& input, IHandlerKeyboard	* handler);
						::nwol::error_t								registerHandler							(SInput& input, IHandlerMouse		* handler);

						// unregisterHandler functions return 0 on success, 1 if the handler was not registered or -1 if the handler pointer is null.
						::nwol::error_t								unregisterHandler		(SInput& input, IHandlerKeyboard	* handler);
						::nwol::error_t								unregisterHandler		(SInput& input, IHandlerMouse		* handler);
} // namespace
#endif // __INPUT_H__9263487236498723649213640918273098__
