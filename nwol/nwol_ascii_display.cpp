#include "nwol_ascii_display.h"
#include "ascii_color.h"
#include "nwol_array.h"

#if defined(__WINDOWS__)
#	include <Windows.h>
#else
#	error "Not implemented."
#endif


#if defined(__WINDOWS__)
					::nwol::error_t						nwol::asciiDisplayTitleSet						(const ::nwol::array_view<const char>& title)															{ return (0 == SetConsoleTitle(title.begin() ? title.begin() : "")) ? -1 : 0; }

struct SWindowsConsoleInfo {
						::CONSOLE_FONT_INFOEX				InfoFontOriginal								= {sizeof(::CONSOLE_FONT_INFOEX)};
						::CONSOLE_FONT_INFOEX				InfoFontCurrent									= {sizeof(::CONSOLE_FONT_INFOEX)};
						::CONSOLE_SCREEN_BUFFER_INFOEX		InfoScreenBufferOriginal						= {sizeof(::CONSOLE_SCREEN_BUFFER_INFOEX)};
						::CONSOLE_SCREEN_BUFFER_INFOEX		InfoScreenBufferCurrent							= {sizeof(::CONSOLE_SCREEN_BUFFER_INFOEX)};
						bool								Created											= false;
};

static				::nwol::array_pod<uint8_t >			g_bufferClearCharacter							= {};
static				::nwol::array_pod<uint16_t>			g_bufferClearColors								= {};
static				::SWindowsConsoleInfo				g_ConsoleInfo									= {};
static constexpr	const ::nwol::SColorRGBA			g_DefaultPalette	[]							= 
	{ ::nwol::ASCII_COLOR_0							
	, ::nwol::ASCII_COLOR_1							
	, ::nwol::ASCII_COLOR_2							
	, ::nwol::ASCII_COLOR_3							
	, ::nwol::ASCII_COLOR_4							
	, ::nwol::ASCII_COLOR_5							
	, ::nwol::ASCII_COLOR_6							
	, ::nwol::ASCII_COLOR_7							
	, ::nwol::ASCII_COLOR_8							
	, ::nwol::ASCII_COLOR_9							
	, ::nwol::ASCII_COLOR_10							
	, ::nwol::ASCII_COLOR_11							
	, ::nwol::ASCII_COLOR_12							
	, ::nwol::ASCII_COLOR_13							
	, ::nwol::ASCII_COLOR_14							
	, ::nwol::ASCII_COLOR_15							
	};

					::nwol::error_t						nwol::asciiDisplayInfo							(::nwol::SASCIIDisplayInfo& displayInfo)																{ 
	::CONSOLE_SCREEN_BUFFER_INFOEX								& csbiInfo										= g_ConsoleInfo.InfoScreenBufferCurrent;
	displayInfo.DisplaySizeMax								= {65535U, 65535U};
	displayInfo.DisplaySizeCurrent							= {(uint32_t)csbiInfo.dwSize.X, (uint32_t)csbiInfo.dwSize.Y};
	displayInfo.ColorWidth									= 24;
	displayInfo.PaletteSize									= 16;
	return 0; 
}

					::nwol::error_t						initWindowsConsoleProperties					(int width, int height, const ::nwol::SColorRGBA* palette)												{
	const ::HANDLE												handleConsoleOut								= ::GetStdHandle(STD_OUTPUT_HANDLE);	// Get console output handle

	::CONSOLE_SCREEN_BUFFER_INFOEX								& csbiInfo										= g_ConsoleInfo.InfoScreenBufferCurrent;
	csbiInfo.dwSize.X										= (::SHORT)width;
	csbiInfo.dwSize.Y										= (::SHORT)height;
	csbiInfo.dwMaximumWindowSize.X							= (::SHORT)width;
	csbiInfo.dwMaximumWindowSize.Y							= (::SHORT)height;

	// We're forced to set up the colors at this point because for some reason the GetConsoleScreenBufferInfoEx() function doesn't return the color table properly,
	// thus they're all 0 when we call SetConsoleScreenBufferInfoEx() causing all the colors to be reset to COLOR_BLACK.
	// Ideally we would have a setConsoleColors() function to do this separately.
	for(uint32_t iColor = 0; iColor < 16; ++iColor)
		csbiInfo.ColorTable[iColor]								= palette[iColor];

	csbiInfo.wAttributes									= ::nwol::ASCII_COLOR_INDEX_WHITE;
	
	uint32_t													newClearSize									= width * height;
	if(newClearSize > g_bufferClearCharacter.size()) {
		g_bufferClearCharacter	.resize(newClearSize);
		g_bufferClearColors		.resize(newClearSize);
	}
	ree_if(FALSE == ::SetConsoleScreenBufferInfoEx(handleConsoleOut, &csbiInfo), "Failed to set console properties.");
	return 0;
}

					::nwol::error_t						nwol::asciiDisplayResize						(uint32_t width, uint32_t height)																		{ return ::initWindowsConsoleProperties(width, height, (const ::nwol::SColorRGBA*)g_ConsoleInfo.InfoScreenBufferCurrent.ColorTable); }
					::nwol::error_t						nwol::asciiDisplayPaletteReset					()																										{ return ::initWindowsConsoleProperties(g_ConsoleInfo.InfoScreenBufferCurrent.dwSize.X, g_ConsoleInfo.InfoScreenBufferCurrent.dwSize.Y, g_DefaultPalette); }
					::nwol::error_t						nwol::asciiDisplayPaletteSet					(const ::nwol::array_view<::nwol::SColorRGBA>& palette)													{
	ree_if(palette.size() < 16, "Palette too small. 16 colors are required for the console palette to be valid.");
	return ::initWindowsConsoleProperties(g_ConsoleInfo.InfoScreenBufferCurrent.dwSize.X, g_ConsoleInfo.InfoScreenBufferCurrent.dwSize.Y, palette.begin());
}

					::nwol::error_t						nwol::asciiDisplaySize							(::nwol::SCoord2<uint32_t>& size)																		{
	const ::CONSOLE_SCREEN_BUFFER_INFOEX						& csbiInfo										= g_ConsoleInfo.InfoScreenBufferCurrent;
	size													= {(uint32_t)csbiInfo.dwSize.X, (uint32_t)csbiInfo.dwSize.Y};
	return 0;
}

					::nwol::error_t						nwol::asciiDisplayClear							(uint8_t character, uint16_t colorRef)																	{
	ree_if(false == ::g_ConsoleInfo.Created, "Cannot clear console if the console wasn't created!");
	ree_if(0 == ::g_bufferClearCharacter.size(), "Cannot clear zero-sized console!");
	if(g_bufferClearCharacter[0] != character)
		memset(&g_bufferClearCharacter[0], character, g_bufferClearCharacter.size());
	const uint16_t												colors[4]										= {colorRef, colorRef, colorRef, colorRef};
	if(g_bufferClearColors[0] != colorRef) {
		 if(g_bufferClearColors.size() % 4) {
			for(uint32_t iCell = 0; iCell < g_bufferClearColors.size(); ++iCell)
				*(uint64_t*)g_bufferClearColors[iCell]						= *(uint64_t*)colors;
		}
		else if(g_bufferClearColors.size() % 2) {
			for(uint32_t iCell = 0; iCell < g_bufferClearColors.size(); ++iCell)
				*(uint32_t*)g_bufferClearColors[iCell]						= *(uint32_t*)colors;
		}
		else {
			for(uint32_t iCell = 0; iCell < g_bufferClearColors.size(); ++iCell)
				g_bufferClearColors[iCell]									= colorRef;
		}
	}

	const ::CONSOLE_SCREEN_BUFFER_INFOEX						& csbiInfo										= g_ConsoleInfo.InfoScreenBufferCurrent;
	uint32_t													sizeFrontBuffer									= csbiInfo.dwSize.X * csbiInfo.dwSize.Y;
	::COORD														offset											= {0, 0};
	::DWORD														dummy											= 0;
	const ::HANDLE												handleConsoleOut								= ::GetStdHandle(STD_OUTPUT_HANDLE);	// Get console output handle
	ree_if(0 == ::WriteConsoleOutputCharacter(handleConsoleOut, (const char*)	g_bufferClearCharacter	.begin(), ::nwol::max(0U, ::nwol::min(g_bufferClearCharacter	.size(), (uint32_t)sizeFrontBuffer)), offset, &dummy ), "How did this happen?");
	ree_if(0 == ::WriteConsoleOutputAttribute(handleConsoleOut,					g_bufferClearColors		.begin(), ::nwol::max(0U, ::nwol::min(g_bufferClearColors		.size(), (uint32_t)sizeFrontBuffer)), offset, &dummy ), "How did this happen?");
	return 0;
}

					::nwol::error_t						nwol::asciiDisplayPresent						(const ::nwol::array_view<const uint8_t>& characters, const ::nwol::array_view<const uint16_t>& colors)	{ 
	ree_if(false == ::g_ConsoleInfo.Created, "Cannot present console if the console wasn't created!");
	const ::HANDLE												handleConsoleOut								= ::GetStdHandle( STD_OUTPUT_HANDLE );	// Get console output handle	
	const ::CONSOLE_SCREEN_BUFFER_INFOEX						& csbiInfo										= g_ConsoleInfo.InfoScreenBufferCurrent;
	uint32_t													sizeFrontBuffer									= csbiInfo.dwSize.X * csbiInfo.dwSize.Y;
	::COORD														offset											= {0, 0};
	::DWORD														dummy											= 0;
	::WriteConsoleOutputCharacter(handleConsoleOut, (const char*)	characters	.begin(), ::nwol::max(0U, ::nwol::min(characters	.size(), (uint32_t)sizeFrontBuffer)), offset, &dummy );
	::WriteConsoleOutputAttribute(handleConsoleOut,					colors		.begin(), ::nwol::max(0U, ::nwol::min(colors		.size(), (uint32_t)sizeFrontBuffer)), offset, &dummy );
	return 0; 
}

					BOOL WINAPI							handlerConsoleRoutine							(_In_ DWORD dwCtrlType)																					{
	switch(dwCtrlType) {
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	default:
		return TRUE;
	}
}

					::nwol::error_t						nwol::asciiDisplayDestroy						()																									{ 
	rww_if(false == g_ConsoleInfo.Created, "Redundant destruction of system console.");
#if defined(__WINDOWS__)
	::SetConsoleCtrlHandler(::handlerConsoleRoutine, FALSE);
	const ::HANDLE												hConsoleOut										= ::GetStdHandle( STD_OUTPUT_HANDLE );
	::SetCurrentConsoleFontEx		(hConsoleOut, FALSE, &g_ConsoleInfo.InfoFontOriginal	);
	::SetConsoleScreenBufferInfoEx	(hConsoleOut, &g_ConsoleInfo.InfoScreenBufferOriginal	);
	::FreeConsole();
	::fclose(stdout);
	::FILE*										
	stream													= 0; ::freopen_s	(&stream, "CONIN$", "r+", stdin);
	stream													= 0; ::fopen_s		(&stream, "CONOUT$", "w+");
#elif defined(__ANDROID__)
#else 
#	error "Not implemented."
#endif
	g_ConsoleInfo.Created									= false;
	return 0; 
}

#else
#	error "Not implemented."
#endif

					::nwol::error_t						nwol::asciiDisplayCreate						(uint32_t frontBufferWidth, uint32_t frontBufferHeight)													{
#if defined(__WINDOWS__)
	::AllocConsole();
	::AttachConsole(::GetCurrentProcessId());
	::EnableMenuItem
		( ::GetSystemMenu(::GetConsoleWindow(), FALSE)
	    , SC_CLOSE
	    , MF_BYCOMMAND | MF_GRAYED 
		);
	const HANDLE												hConsoleIn										= ::GetStdHandle(STD_INPUT_HANDLE);
    DWORD														mode											= 0;
    ::GetConsoleMode(hConsoleIn, &mode);
    mode													&= ~(DWORD)ENABLE_QUICK_EDIT_MODE;
    ::SetConsoleMode(hConsoleIn, mode);

	FILE*												
	stream													= 0;	::freopen_s(&stream, "CONOUT$", "w+", stdout);
	stream													= 0;	::freopen_s(&stream, "CONIN$", "r+", stdin);

	SetConsoleTitle("No Workflow Overhead Console");
	::SetConsoleCtrlHandler(::handlerConsoleRoutine, TRUE);
	const HANDLE												hConsoleOut										= ::GetStdHandle( STD_OUTPUT_HANDLE );
	::GetCurrentConsoleFontEx		( hConsoleOut, TRUE	, &g_ConsoleInfo.InfoFontOriginal			);
	::GetConsoleScreenBufferInfoEx	( hConsoleOut		, &g_ConsoleInfo.InfoScreenBufferOriginal	);
	g_ConsoleInfo.InfoFontCurrent							= g_ConsoleInfo.InfoFontOriginal			;
	g_ConsoleInfo.InfoScreenBufferCurrent					= g_ConsoleInfo.InfoScreenBufferOriginal	;

	::CONSOLE_FONT_INFOEX										infoFont										= {sizeof(::CONSOLE_FONT_INFOEX)};
	infoFont.dwFontSize.X									= 8;
	infoFont.dwFontSize.Y									= 12;
	::wcscpy_s(infoFont.FaceName, L"Terminal");
	::SetCurrentConsoleFontEx(hConsoleOut, FALSE, &infoFont);

	::CONSOLE_SCREEN_BUFFER_INFOEX								& csbiInfo										= g_ConsoleInfo.InfoScreenBufferCurrent;
	csbiInfo.srWindow.Top									= 10;
	csbiInfo.srWindow.Left									= 10;
	csbiInfo.srWindow.Right									= 800;
	csbiInfo.srWindow.Bottom								= 600;
	::initWindowsConsoleProperties(frontBufferWidth, frontBufferHeight, g_DefaultPalette);
	g_ConsoleInfo.Created									= true;
#elif defined(__ANDROID__)
#else
#	error "Not implemented."
#endif
	return 0; 
}
