#include "nwol_misc.h"
#include "label.h"
#include "ascii_target.h"
#include <cstring>

#ifndef __TEXT_H__923649827346982746982346298734623__
#define __TEXT_H__923649827346982746982346298734623__

namespace nwol
{
	static inline				int32_t	lineToRect				( char* rectangleTopLeft, uint32_t width, uint32_t height, int32_t offsetLine, int32_t offsetColumn, nwol::ALIGN_SCREEN align, const char* text, int32_t charCount = -1 )	{
		int32_t									textLen					= (int32_t)strlen(text);
		if( textLen > charCount && charCount >= 0 )
			textLen								= charCount;

		return valueToRect(rectangleTopLeft, width, height, offsetLine, offsetColumn, align, text, textLen);
	}

	template<typename... _Args>
								int32_t	printfToRect			( char* rectangleTopLeft, uint32_t width, uint32_t height, int32_t offsetLine, int32_t offsetColumn, nwol::ALIGN_SCREEN align, const char* format, _Args&&... args )		{
		char									precookStr[1024]		= {};
		int32_t									precookLen				= sprintf_s(precookStr, format, args...);
		return valueToRect(rectangleTopLeft, width, height, offsetLine, offsetColumn, align, precookStr, precookLen);
	}

	static						int32_t	lineToRectColored		( char* display, uint32_t width, uint32_t height, uint16_t* textAttributes, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, nwol::ALIGN_SCREEN align, const char* text, uint32_t charCount = 0xFFFFFFFF ) {
		int32_t									actualX					= lineToRect(display, width, height, offsetLine, offsetColumn, align, text, charCount);
		uint32_t								stringLen				= (uint32_t)strlen(text);
		uint32_t								cellCount				= ((stringLen < charCount) ? stringLen : charCount);
		uint32_t								maxCount				= (uint32_t)width-actualX;
		cellCount							= (cellCount < maxCount) ? cellCount : maxCount;
		for(uint32_t i=0; i<cellCount; ++i)
			textAttributes[offsetLine*width+actualX+i] = messageColor;
		return actualX;
	}

	template<typename... _Args>
								int32_t	printfToRectColored		(char* display, uint32_t width, uint32_t height, uint16_t* textAttributes, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, nwol::ALIGN_SCREEN align, const char* format, _Args&&... args) {
		char									precookStr[1024]		= {};
		int32_t									precookLen				= sprintf_s(precookStr, format, args...);
		int32_t									actualX					= lineToRectColored(display, width, height, textAttributes, messageColor, offsetLine, offsetColumn, align, precookStr);
		return actualX;
	}


	static inline				int32_t	lineToRectColored		(SASCIITarget& target, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, nwol::ALIGN_SCREEN align, const char* text, uint32_t charCount = 0xFFFFFFFF)		{
		return lineToRectColored	(target.Text.begin(), target.Width(), target.Height(), target.Attributes.begin(), messageColor, offsetLine, offsetColumn, align, text, charCount);
	}
	template<typename... _Args>	int32_t	printfToRectColored		(SASCIITarget& target, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, nwol::ALIGN_SCREEN align, const char* format, _Args&&... args)						{
		return printfToRectColored	(target.Text.begin(), target.Width(), target.Height(), target.Attributes.begin(), messageColor, offsetLine, offsetColumn, align, format, args...);
	}
	template <size_t _Size>		void	resetCursorString		(char (&textContainer)[_Size])																																				{ textContainer[textContainer[1] = 0] = '_';	}
	template <size_t _Size> 	bool	getMessageSlow			(char (&message)[_Size], const char* textToPrint, uint32_t sizeToPrint, double lastFrameSeconds, double limit = 0.025f)														{
		static	float							nextTick				= 0.0f;
		static	uint32_t						tickCount				= 0;

		uint32_t								mesLen					= (int32_t)strlen(message);
		if(0 == mesLen)
			return true;

		if(memcmp(message, textToPrint, ::nwol::max(0U, ::nwol::min(sizeToPrint, mesLen-1)))) {
			resetCursorString(message);
			mesLen								= (uint32_t)strlen(message);
		}

		mesLen								= ((mesLen+1) > (_Size-1)) ? _Size-2 : mesLen;

		nextTick							+= (float)lastFrameSeconds;
		if(nextTick > limit) {
			tickCount++;
			uint32_t								cursorIndex				= mesLen-1;
			if(cursorIndex < sizeToPrint) {
				message[cursorIndex]				= textToPrint[cursorIndex];
				message[mesLen]						= '_';
				message[cursorIndex+2]				= 0;
				nextTick							= 0.0f;
			}
			else if(0 == (tickCount % 20))
				message[cursorIndex]				= (message[cursorIndex] == ' ') ? '_' : ' ';
		}

		return ( mesLen-1 == sizeToPrint );
	}

	template <size_t _Size> 
	static inline				bool	getMessageSlow			(char (&message)[_Size], const ::nwol::glabel& textToPrint, double lastFrameSeconds)		{ return getMessageSlow(message, textToPrint.c_str(), textToPrint.size(), lastFrameSeconds); }

} // namespace

#endif // __TEXT_H__923649827346982746982346298734623__
