/// Copyright 2014-2017 - asm128
#include "ascii_screen.h"
#include "ascii_color.h"
#include "nwol_debug.h"
#include "nwol_memory.h"
#include "nwol_io.h"
#include "nwol_safe.h"

#include <stdlib.h>
#include <string.h>

#if defined(__WINDOWS__)
#	include <Windows.h>
#endif

static struct SASCIIScreen {
	char								* ASCIIBackBuffer					= 0;
	char								* ASCIIFrontBuffer					= 0;
	uint16_t							* ColorBackBuffer					= 0;
	uint16_t							* ColorFrontBuffer					= 0;	
	int									BackBufferWidth						= DEFAULT_ASCII_SCREEN_WIDTH	;
	int									BackBufferHeight					= DEFAULT_ASCII_SCREEN_HEIGHT	;
	bool								bCreated							= false;
	// platform
#if defined(__WINDOWS__)
	CONSOLE_SCREEN_BUFFER_INFOEX		InfoScreenBufferOriginal			= {};
	CONSOLE_SCREEN_BUFFER_INFOEX		InfoScreenBufferCurrent				= {};
	CONSOLE_FONT_INFOEX					InfoFontOriginal					= {};
	CONSOLE_FONT_INFOEX					InfoFontCurrent						= {};
#endif
										~SASCIIScreen						()																		{
		if( bCreated )	
			::nwol::shutdownASCIIScreen();
		::nwol::safe_nwol_free( ASCIIBackBuffer		);
		::nwol::safe_nwol_free( ASCIIFrontBuffer	);
		::nwol::safe_nwol_free( ColorBackBuffer		);
		::nwol::safe_nwol_free( ColorFrontBuffer	);
	}
}									__g_ASCIIScreen;	// Main instance (Windows applications seem to support only one console per process, as well as many custom smaller systems)

#if defined(__WINDOWS__)
BOOL WINAPI							HandlerRoutine						(_In_ DWORD dwCtrlType)													{
	switch(dwCtrlType) {
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	default:
		return TRUE;
	}
}
#endif

void								nwol::shutdownASCIIScreen			()																		{
	if(false == __g_ASCIIScreen.bCreated)
		return; 

#if defined(__WINDOWS__)
	SetConsoleCtrlHandler(HandlerRoutine, FALSE);

	const HANDLE							hConsoleOut						= GetStdHandle( STD_OUTPUT_HANDLE );
	SetCurrentConsoleFontEx			(hConsoleOut, FALSE, &__g_ASCIIScreen.InfoFontOriginal	);
	SetConsoleScreenBufferInfoEx	(hConsoleOut, &__g_ASCIIScreen.InfoScreenBufferOriginal	);

	FreeConsole();
#elif defined(__ANDROID__)
#else 
#error "Not implemented."
#endif
	__g_ASCIIScreen.bCreated			= false;
}

char*								nwol::getASCIIBackBuffer			()																		{
	if( 0 == __g_ASCIIScreen.ASCIIBackBuffer )
		::nwol::clearASCIIBackBuffer(' ', COLOR_WHITE);
	return __g_ASCIIScreen.ASCIIBackBuffer;
}

int32_t								nwol::getASCIIBackBuffer			(::nwol::SASCIITarget& target)											{
	if( 0 == __g_ASCIIScreen.ASCIIBackBuffer )
		::nwol::clearASCIIBackBuffer(' ', COLOR_WHITE);
	target.Text							= __g_ASCIIScreen.ASCIIBackBuffer ? ::nwol::grid_view<char_t	>(__g_ASCIIScreen.ASCIIBackBuffer, ::nwol::getASCIIBackBufferWidth(), getASCIIBackBufferHeight()) : ::nwol::grid_view<char_t	>{};
	target.Attributes					= __g_ASCIIScreen.ColorBackBuffer ? ::nwol::grid_view<uint16_t	>(__g_ASCIIScreen.ColorBackBuffer, ::nwol::getASCIIBackBufferWidth(), getASCIIBackBufferHeight()) : ::nwol::grid_view<uint16_t	>{};
	return __g_ASCIIScreen.bCreated ? 0 : -1;
}

int32_t								nwol::getASCIIFrontBuffer			(::nwol::SASCIITarget& target)											{
	target.Text							= __g_ASCIIScreen.ASCIIFrontBuffer ? ::nwol::grid_view<char_t	>(__g_ASCIIScreen.ASCIIFrontBuffer, ::nwol::getASCIIBackBufferWidth(), getASCIIBackBufferHeight()) : ::nwol::grid_view<char_t	>{};
	target.Attributes					= __g_ASCIIScreen.ColorFrontBuffer ? ::nwol::grid_view<uint16_t	>(__g_ASCIIScreen.ColorFrontBuffer, ::nwol::getASCIIBackBufferWidth(), getASCIIBackBufferHeight()) : ::nwol::grid_view<uint16_t	>{};
	return 0;
}

void									nwol::setASCIIScreenTitle		(const char_t* title)													{ 
#if defined(__WINDOWS__)
	SetConsoleTitle(title);		
#elif defined(__ANDROID__)
#else
#	error "Not implemented."
#endif
}
const char_t*							nwol::getASCIIFrontBuffer		()																		{ return __g_ASCIIScreen.ASCIIFrontBuffer;	}
uint16_t*								nwol::getASCIIColorBackBuffer	()																		{ return __g_ASCIIScreen.ColorBackBuffer;	}
const uint16_t*							nwol::getASCIIColorFrontBuffer	()																		{ return __g_ASCIIScreen.ColorFrontBuffer;	}
uint32_t								nwol::getASCIIBackBufferWidth	()																		{ return __g_ASCIIScreen.BackBufferWidth;	}
uint32_t								nwol::getASCIIBackBufferHeight	()																		{ return __g_ASCIIScreen.BackBufferHeight;	}

void									nwol::swapASCIIBuffers			()																		{
	char										* bB							= __g_ASCIIScreen.ASCIIBackBuffer ;
	__g_ASCIIScreen.ASCIIBackBuffer 		= __g_ASCIIScreen.ASCIIFrontBuffer;
	__g_ASCIIScreen.ASCIIFrontBuffer		= bB;

	uint16_t									* cBB							= __g_ASCIIScreen.ColorBackBuffer ;
	__g_ASCIIScreen.ColorBackBuffer			= __g_ASCIIScreen.ColorFrontBuffer;
	__g_ASCIIScreen.ColorFrontBuffer		= cBB;
}

void									nwol::presentASCIIBackBuffer	()																		{
	swapASCIIBuffers();
	presentASCIIFrontBuffer();
}

void									nwol::presentASCIIFrontBuffer	()																		{
	int32_t										screenSize						= __g_ASCIIScreen.BackBufferWidth*__g_ASCIIScreen.BackBufferHeight;
	screenSize = (screenSize > 4) ? screenSize : 4;
#if defined(__WINDOWS__)
	const ::HANDLE								hConsoleOut						= GetStdHandle( STD_OUTPUT_HANDLE );
	::CONSOLE_SCREEN_BUFFER_INFO				csbiInfo						= {};
    GetConsoleScreenBufferInfo( hConsoleOut, &csbiInfo );
	::COORD										Coords							= {0, csbiInfo.dwSize.Y-(SHORT)__g_ASCIIScreen.BackBufferHeight};
	DWORD										dummy							= 0;
	if( __g_ASCIIScreen.ASCIIFrontBuffer ) {
		WriteConsoleOutputAttribute ( hConsoleOut, __g_ASCIIScreen.ColorFrontBuffer, screenSize, Coords, &dummy );
		WriteConsoleOutputCharacter ( hConsoleOut, __g_ASCIIScreen.ASCIIFrontBuffer, screenSize, Coords, &dummy );
	}
#elif defined(__ANDROID__)
#else
	if( __g_ASCIIScreen.ASCIIFrontBuffer ) {
		__g_ASCIIScreen.ASCIIFrontBuffer[(__g_ASCIIScreen.BackBufferWidth*__g_ASCIIScreen.BackBufferHeight)] = 0;
		fprintf(stdout, "\n%s", __g_ASCIIScreen.ASCIIFrontBuffer);
	}
#endif
}

void									nwol::clearASCIIBackBuffer		(char_t value, uint16_t colorValue)										{
	int32_t										screenSize						= __g_ASCIIScreen.BackBufferWidth * __g_ASCIIScreen.BackBufferHeight;
	screenSize								= (screenSize > 4) ? screenSize : 4;
	if( 0 == __g_ASCIIScreen.ASCIIBackBuffer ) {
#if defined(__WINDOWS__)
		const ::HANDLE								hConsoleOut						= GetStdHandle( STD_OUTPUT_HANDLE );
		::CONSOLE_SCREEN_BUFFER_INFOEX				csbiInfo						= {};
		GetConsoleScreenBufferInfoEx( hConsoleOut, &csbiInfo );
		csbiInfo.dwCursorPosition.Y				= csbiInfo.dwSize.Y-1;
		csbiInfo.dwSize.X						= (SHORT)__g_ASCIIScreen.BackBufferWidth;
		csbiInfo.dwSize.Y						= (SHORT)__g_ASCIIScreen.BackBufferHeight;
		SetConsoleScreenBufferInfoEx( hConsoleOut, &csbiInfo );
		COORD										home							= { 0, 0 };
#elif defined(__ANDROID__)
#else
#	error "Not implemented."
#endif 
		__g_ASCIIScreen.ASCIIBackBuffer			= (char*)		::nwol::nwol_malloc( sizeof( char )*screenSize+1	);	// We're going to draw our map in this array. the assignment of empty brackets = {} initializes all chars in the array to 0
		__g_ASCIIScreen.ColorBackBuffer			= (uint16_t*)	::nwol::nwol_malloc( sizeof( uint16_t )*screenSize	);	
	}

	if( __g_ASCIIScreen.ASCIIBackBuffer)
		memset(__g_ASCIIScreen.ASCIIBackBuffer, value, sizeof( char )*screenSize);

	if(__g_ASCIIScreen.ColorBackBuffer)
		for(int32_t i=0; i<screenSize; ++i)
			__g_ASCIIScreen.ColorBackBuffer[i]		= colorValue;
}
	
void									createConsole				()																		{
#if defined(__WINDOWS__)
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE*										
	stream									= 0;	freopen_s(&stream, "CONOUT$", "w+", stdout);
	stream									= 0;	freopen_s(&stream, "CONIN$", "r+", stdin);

	SetConsoleTitle("No Workflow Overhead Console");
	SetConsoleCtrlHandler(HandlerRoutine, TRUE);
#elif defined(__ANDROID__)
#else
#	error "Not implemented."
#endif
	__g_ASCIIScreen.bCreated				= true;
}

void									nwol::initASCIIScreen		(uint32_t width, uint32_t height)										{
	if(false == __g_ASCIIScreen.bCreated)
		createConsole();

	__g_ASCIIScreen.BackBufferWidth 		= width;
	__g_ASCIIScreen.BackBufferHeight		= height;
#if defined(__WINDOWS__)
	const HANDLE								hConsoleOut					= GetStdHandle( STD_OUTPUT_HANDLE );

	EnableMenuItem (	GetSystemMenu ( GetConsoleWindow(), FALSE)
	    ,				SC_CLOSE
	    ,				MF_BYCOMMAND | MF_GRAYED 
		);
	GetConsoleScreenBufferInfoEx( hConsoleOut, &__g_ASCIIScreen.InfoScreenBufferOriginal );
	GetConsoleScreenBufferInfoEx( hConsoleOut, &__g_ASCIIScreen.InfoScreenBufferCurrent );
	CONSOLE_SCREEN_BUFFER_INFOEX				& csbiInfo					= __g_ASCIIScreen.InfoScreenBufferCurrent;
	//csbiInfo.dwCursorPosition.Y = csbiInfo.dwSize.Y-1;
	csbiInfo.cbSize							= sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	//csbiInfo.wAttributes
	csbiInfo.dwSize.X						= (SHORT)__g_ASCIIScreen.BackBufferWidth ;
	csbiInfo.dwSize.Y						= (SHORT)__g_ASCIIScreen.BackBufferHeight;
	csbiInfo.dwMaximumWindowSize.X			= (SHORT)__g_ASCIIScreen.BackBufferWidth ;
	csbiInfo.dwMaximumWindowSize.Y			= (SHORT)__g_ASCIIScreen.BackBufferHeight;
	csbiInfo.bFullscreenSupported			= TRUE;
	csbiInfo.srWindow.Top					= 10;
	csbiInfo.srWindow.Left					= 10;
	csbiInfo.srWindow.Right					= 800;
	csbiInfo.srWindow.Bottom				= 600;

	csbiInfo.ColorTable[0 ]					= COLOR_0 ;
	csbiInfo.ColorTable[1 ]					= COLOR_1 ;
	csbiInfo.ColorTable[2 ]					= COLOR_2 ;
	csbiInfo.ColorTable[3 ]					= COLOR_3 ;
	csbiInfo.ColorTable[4 ]					= COLOR_4 ;
	csbiInfo.ColorTable[5 ]					= COLOR_5 ;
	csbiInfo.ColorTable[6 ]					= COLOR_6 ;
	csbiInfo.ColorTable[7 ]					= COLOR_7 ;
	csbiInfo.ColorTable[8 ]					= COLOR_8 ;
	csbiInfo.ColorTable[9 ]					= COLOR_9 ;
	csbiInfo.ColorTable[10]					= COLOR_10;
	csbiInfo.ColorTable[11]					= COLOR_11;
	csbiInfo.ColorTable[12]					= COLOR_12;
	csbiInfo.ColorTable[13]					= COLOR_13;
	csbiInfo.ColorTable[14]					= COLOR_14;
	csbiInfo.ColorTable[15]					= COLOR_15;
	csbiInfo.wAttributes					= COLOR_WHITE;
	
	SetConsoleScreenBufferInfoEx( hConsoleOut, &csbiInfo );

	GetCurrentConsoleFontEx( hConsoleOut, TRUE, &__g_ASCIIScreen.InfoFontOriginal );
	GetCurrentConsoleFontEx( hConsoleOut, TRUE, &__g_ASCIIScreen.InfoFontCurrent );

	CONSOLE_FONT_INFOEX							infoFont;
	infoFont.cbSize							= sizeof(CONSOLE_FONT_INFOEX);
	infoFont.dwFontSize.X					= 8;
	infoFont.dwFontSize.Y					= 12;
	infoFont.FontFamily						= 0;
	infoFont.FontWeight						= 0;
	infoFont.nFont							= 0;
	wcscpy_s(infoFont.FaceName, L"Terminal");
	SetCurrentConsoleFontEx(hConsoleOut, FALSE, &infoFont);

	const HANDLE								hConsoleIn					= GetStdHandle(STD_INPUT_HANDLE);
    DWORD										mode;
    GetConsoleMode(hConsoleIn, &mode);
    mode									&= ~(DWORD)ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(hConsoleIn, mode);
#elif defined(__ANDROID__)
#else
#	error "Not implemented."
#endif 
}
