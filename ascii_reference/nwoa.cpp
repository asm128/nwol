#include "nwoa.h"

#include "text.h"
#include "ascii_screen.h"
#include "gui.h"

#include "runtime_impl.h"
#include "nwol_debug.h"

#include <thread>
#include <process.h>
#include <ctime>

#include <algorithm>

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(SApplication, "Tic Tac Toe", 0, 1);

				int32_t						setupGUI							(::SApplication& instanceApp)																			{ 
    info_printf("%s", "Initializing Graphical User Interface.");
	::nwol::SGUI									& guiSystem							= instanceApp.GUI;

	// Create exit button
	static const ::nwol::glabel						labelExit							= "Exit";
	::nwol::SGUIControl								newControl;
	newControl.AreaASCII						= {{1, 1}, {(int32_t)labelExit.size(), 1}}	;
	newControl.Text								= labelExit									;
	nwol_necall(::nwol::createControl(guiSystem, newControl), "%s: \"%s\".", "Failed to create control", labelExit.begin());

	// Create game board buttons: each board cell is a button.
	newControl.AlignArea						= (::nwol::ALIGN_SCREEN)(::nwol::SCREEN_CENTER | ::nwol::SCREEN_VCENTER)		;
	char blech[] = "   ";
	for(int32_t y = 0; y < 8; ++y) 
		for(int32_t x = 0; x < 32; ++x) {
			newControl.AreaASCII						= {{-48 + x * 3, -16 + y}, {3, 1}}										;
			blech[1]									= char(y*32+x)																;
			newControl.Text								= blech																	;		
			::nwol::SControlTextColorASCII					& colorsConsole						= newControl.TextColorsASCII	;
			colorsConsole.Color.Background				= ((y * 3 + x) % 2) ? COLOR_DARKGREY : COLOR_WHITE						;
			colorsConsole.Color.Foreground				= COLOR_BLACK															;
			colorsConsole.ColorPressed					= {COLOR_DARKGREY, COLOR_YELLOW}										;
			//newControl.AlignText						= ::nwol::SCREEN_CENTER												this is not working yet
			error_if(errored(::nwol::createControl(guiSystem, newControl)), "%s", "Failed to create control.");
		}

	for(int32_t y = 0; y < 8; ++y) 
		for(int32_t x = 0; x < 32; ++x) {
			newControl.AreaASCII						= {{-78 + x * 5, 0 + y}, {5, 1}}										;
			newControl.Text								= " " + ::std::to_string(y*32+x)										;
			::nwol::SControlTextColorASCII					& colorsConsole						= newControl.TextColorsASCII	;
			colorsConsole.Color.Background				= ((y * 5 + x) % 2) ? COLOR_DARKGREY : COLOR_WHITE						;
			colorsConsole.Color.Foreground				= COLOR_BLACK															;
			colorsConsole.ColorPressed					= {COLOR_DARKGREY, COLOR_YELLOW}										;
			//newControl.AlignText						= ::nwol::SCREEN_CENTER												this is not working yet
			error_if(errored(::nwol::createControl(guiSystem, newControl)), "%s", "Failed to create control.");
		}

	// Create restart button
	static const ::nwol::glabel						labelRestart						= "Restart";
	newControl									= {}																	;
	newControl.AlignArea						= (::nwol::ALIGN_SCREEN)(::nwol::SCREEN_CENTER | ::nwol::SCREEN_BOTTOM)	;
	newControl.AreaASCII						= {{0, 1}, {(int32_t)labelRestart.size(), 1}}							;
	newControl.Text								= labelRestart															;

	nwol_necall(::nwol::createControl(guiSystem, newControl), "%s: \"%s\".", "Failed to create control", labelRestart.begin());
    info_printf("%s", "Done initializing GUI.");
	return 0;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
				int32_t						cleanup								(::SApplication& /*instanceApp*/)																		{
    info_printf("%s", "Cleaning up resources...");
	::nwol::shutdownASCIIScreen();

	info_printf("%s", "Done cleaning up resources.");
	return 0; 
}

				int32_t						setup								(::SApplication& instanceApp)																			{ 
    info_printf("%s", "Initializing application...");
	srand((unsigned int)time(0));

	::nwol::SGUI									& guiSystem							= instanceApp.GUI;
	int32_t											screenWidth							= guiSystem.TargetSizeASCII.x;
	int32_t											screenHeight						= guiSystem.TargetSizeASCII.y;
	::nwol::initASCIIScreen(screenWidth, screenHeight);
	char															moduleTitle[240]						= {};
	uint8_t															moduleTitleLen							= (uint8_t)::nwol::size(moduleTitle);
	nwol_necall(::nwol_moduleTitle(moduleTitle, &moduleTitleLen), "If this fails then something weird is going on.");
	::nwol::setASCIIScreenTitle(moduleTitle);

	nwol_necall(::setupGUI		(instanceApp), "%s", "Failed to set up GUI."			);

    info_printf("%s", "Done initializing application.");
	return 0; 
}

#if defined(__WINDOWS__)
#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif
#else 
#error "Unsupported platform. Define key code for this platform in order to continue."
#endif

				int32_t						update								(::SApplication& instanceApp, bool exitRequested)														{
	verbose_printf("%s", "Enter.");
	if(exitRequested)
		return ::nwol::APPLICATION_STATE_EXIT;

	// Update subsystems
	::nwol::SInput									& inputSystem						= instanceApp.Input;	::nwol::pollInput(inputSystem);
	::nwol::SGUI									& guiSystem							= instanceApp.GUI;		::nwol::updateGUI(guiSystem, inputSystem);
	::nwol::SGUIControlTable						& guiControls						= guiSystem.Controls;	

	// Update game
	::nwol::array_pod<::nwol::CONTROL_FLAG>			& controlFlags						= guiControls.ControlFlags;

	static uint8_t									tickDelay							= 0;
	bool											handledControlEvent					= false;
	int32_t											otherControlIndex;
	for(uint32_t iControl = 0, controlCount = controlFlags.size(); iControl < controlCount; ++iControl) {
		if(::nwol::bit_true(controlFlags[iControl], ::nwol::CONTROL_FLAG_EXECUTE)) {
			info_printf("Execute %u.", iControl);
			switch(iControl) {
			case 0:		return ::nwol::APPLICATION_STATE_EXIT; // Exit button clicked.
			default:	// Board cell clicked 
				for(int32_t y = 0; y < 8; ++y) 
					for(int32_t x = 0; x < 32; ++x) {
						int32_t											iOtherControl						= y * 32 + x + 1;
						guiControls.TextColorsASCII[iOtherControl+256].Color.Foreground	= guiControls.TextColorsASCII[iOtherControl].Color.Foreground	= COLOR_BLACK;
						guiControls.TextColorsASCII[iOtherControl+256].Color.Background	= guiControls.TextColorsASCII[iOtherControl].Color.Background	= ((y * 5 + x) % 2) ? COLOR_DARKGREY : COLOR_WHITE;
					}

				otherControlIndex																	= (iControl > 256) ? iControl - 256 : iControl + 256;
				guiControls.TextColorsASCII[otherControlIndex]	.Color	.Foreground					= COLOR_DARKCYAN;
				guiControls.TextColorsASCII[iControl]			.Color	.Foreground					= COLOR_DARKCYAN;
				guiControls.TextColorsASCII[otherControlIndex]	.Color	.Background					= COLOR_YELLOW;
				guiControls.TextColorsASCII[iControl]			.Color	.Background					= COLOR_YELLOW;
				handledControlEvent							= true;
				break;
			}
		}
		if(handledControlEvent)
			break;
	}

	tickDelay									+= 2;	// Always decrease AI delay
	sizeof(::SApplication);
    verbose_printf("%s", "Exit.");
	return 0; 
}

				int32_t						render							(::SApplication& instanceApp)																				{
    verbose_printf("%s", "Enter.");

	::nwol::clearASCIIBackBuffer(' ', COLOR_WHITE);

	::nwol::SASCIITarget							actualScreen					= {};
	nwol_necall(::nwol::getASCIIBackBuffer(actualScreen), "%s", "Failed to get ASCII target!");

	::nwol::renderGUIASCII(actualScreen, instanceApp.GUI);
	::nwol::presentASCIIBackBuffer();
	verbose_printf("%s", "Exit.");
	return 0; 
}
