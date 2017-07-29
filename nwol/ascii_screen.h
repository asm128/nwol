/// Copyright 2014-2017 - asm128
#include "ascii_target.h"

#ifndef ASCIISCREEN_H_20983472903740928374234
#define ASCIISCREEN_H_20983472903740928374234

#define DEFAULT_ASCII_SCREEN_HEIGHT		83
#define DEFAULT_ASCII_SCREEN_WIDTH		((uint32_t)(DEFAULT_ASCII_SCREEN_HEIGHT*2.666666f))

namespace nwol
{
	void			initASCIIScreen					(uint32_t width = DEFAULT_ASCII_SCREEN_WIDTH, uint32_t height = DEFAULT_ASCII_SCREEN_HEIGHT);
	void			shutdownASCIIScreen				();
	void			setASCIIScreenTitle				(const char_t* title);

	char*			getASCIIBackBuffer				();
	const char*		getASCIIFrontBuffer				();

	int32_t			getASCIIBackBuffer				(SASCIITarget& target);
	int32_t			getASCIIFrontBuffer				(SASCIITarget& target);

	uint16_t*		getASCIIColorBackBuffer			();
	const uint16_t*	getASCIIColorFrontBuffer		();

	uint32_t		getASCIIBackBufferWidth			();
	uint32_t		getASCIIBackBufferHeight		();

	//int32_t setASCIIBackBufferWidth( uint32_t backBufferWidth );
	//int32_t setASCIIBackBufferHeight( uint32_t backBufferHeight );

	void			swapASCIIBuffers				();
	void			clearASCIIBackBuffer			(char_t value, uint16_t colorValue=15);
	void			presentASCIIFrontBuffer			();
	void			presentASCIIBackBuffer			();

};

#endif // ASCIISCREEN_H_20983472903740928374234
