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
	::nwol::error_t									errMy								= ::nwol::createControl(guiSystem, newControl);
	reterr_error_if_errored(errMy, "%s: \"%s\".", "Failed to create control", labelExit.begin());

	// Create game board buttons: each board cell is a button.
	newControl.AlignArea						= (::nwol::ALIGN_SCREEN)(::nwol::SCREEN_CENTER | ::nwol::SCREEN_VCENTER)		;
	for(int32_t y = 0; y < 3; ++y) 
		for(int32_t x = 0; x < 3; ++x) {
			newControl.AreaASCII						= {{-3 + x * 3, -3 + y}, {3, 1}}										;
			newControl.Text								= ::nwol::glabel::statics().space										;
			::nwol::SControlTextColorASCII					& colorsConsole						= newControl.TextColorsASCII	;
			colorsConsole.Color.Background				= ((y * 3 + x) % 2) ? COLOR_DARKGREY : COLOR_WHITE						;
			colorsConsole.Color.Foreground				= COLOR_YELLOW															;
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

	errMy										= ::nwol::createControl(guiSystem, newControl);
	reterr_error_if_errored(errMy, "%s: \"%s\".", "Failed to create control", labelRestart.begin());

    info_printf("%s", "Done initializing GUI.");
	return 0;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
				int32_t						cleanup								(::SApplication& instanceApp)																			{
    info_printf("%s", "Cleaning up resources...");
	::nwol::shutdownASCIIScreen();

	::networkDisable(instanceApp);
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
	::nwol::setASCIIScreenTitle(appTitle());

	::nwol::error_t									
	//errMy										= ::setupDB			(instanceApp);	reterr_error_if_errored(errMy, "%s", "Failed to set up database."	);
	//errMy										= ::setupREST		(instanceApp);	reterr_error_if_errored(errMy, "%s", "Failed to set up REST."		);
	errMy										= ::setupGUI		(instanceApp);	reterr_error_if_errored(errMy, "%s", "Failed to set up GUI."		);

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
	::ttt::TicTacToe								& game								= instanceApp.Game;
	::ttt::TicTacToeBoard32							& gameBoard							= game.Board;
	::nwol::array_pod<::nwol::CONTROL_FLAG>			& controlFlags						= guiControls.ControlFlags;

	const ::ttt::PLAYER_CONTROL						currentPlayer						= gameBoard.GetPlayerControlCurrent();
	::ttt::CellPick									cellPicked;
	static uint8_t									tickDelay							= 0;
	bool											handledControlEvent					= false;
	for(uint32_t iControl = 0, controlCount = controlFlags.size(); iControl < controlCount; ++iControl) {
		if(::nwol::bit_true(controlFlags[iControl], ::nwol::CONTROL_FLAG_EXECUTE)) {
			info_printf("Execute %u.", iControl);
			switch(iControl) {
			case 0:		return ::nwol::APPLICATION_STATE_EXIT; // Exit button clicked.
			case 10:	// Restart button clicked.
				game.Restart();

				for(int32_t y=0; y<3; ++y) {		// Clear board controls.
					for(int32_t x=0; x<3; ++x){
						const int										 controlIndex						= (y * 3 + x) + 1;
						guiControls.Text								[controlIndex]						= ::nwol::glabel::statics().space;
						guiControls.TextColorsASCII						[controlIndex].Color.Foreground		= COLOR_YELLOW;
						guiControls.TextColorsASCII						[controlIndex].ColorPressed			= {COLOR_DARKGREY, COLOR_YELLOW};
					}
				}
				info_printf("%s", "Game restarted.");
				handledControlEvent							= true;
				break;

			default:	// Board cell clicked 
				if(::ttt::PLAYER_CONTROL_HUMAN == currentPlayer && gameBoard.MovesLeft && !gameBoard.Winner) {
					const uint8_t									cellIndex						= (uint8_t)(iControl - 1);
					cellPicked									= {::ttt::CELL_VALUE_EMPTY, cellIndex, (uint8_t)gameBoard.PlayerIndex};
					if(::ttt::CELL_VALUE_EMPTY == gameBoard.GetCellValue(cellIndex)) {
						cellPicked									= instanceApp.Game.Tick(cellIndex);
						tickDelay									= 2;
					}
					const int32_t									playerNumber					= cellPicked.IndexPlayer + 1;
					const char										* textAction					= cellPicked.Value ? "Player %u played {%u, %u}." : "Player %u cannot move to {%u, %u}.";
					always_printf(textAction, playerNumber, cellPicked.Row, cellPicked.Column);
				}
				handledControlEvent							= true;
				break;
			}
		}
		if(handledControlEvent)
			break;
	}

	// If the human didn't exit the game and it's not its turn, then let the computer play
	if(currentPlayer != ::ttt::PLAYER_CONTROL_HUMAN && gameBoard.MovesLeft && !gameBoard.Winner && !tickDelay && !cellPicked.Value)
		cellPicked									= game.Tick(0);

	if(cellPicked.Value) {	// If a cell was picked and its value changed, we need to change the colors of the cell control
		static const ::nwol::glabel						symbols[]								= {" X ", " O "};
		const uint32_t									controlIndex							= cellPicked.IndexCell + 1;

		guiControls.TextColorsASCII	[controlIndex].Color		.Foreground		= 
		guiControls.TextColorsASCII	[controlIndex].ColorPressed	.Background		= cellPicked.IndexPlayer ? COLOR_RED : COLOR_BLUE;
		guiControls.Text			[controlIndex]								= symbols[cellPicked.IndexPlayer];
	}

	tickDelay									+= 2;	// Always decrease AI delay

	sizeof(::SApplication);
	sizeof(ttt::TicTacToe);
	sizeof(ttt::CellCoord);
	sizeof(ttt::CellPick);
    verbose_printf("%s", "Exit.");
	return 0; 
}

template<uint32_t _screenWidth, uint32_t _screenHeight>
				void						bltASCIIScreen					(const ttt::ScreenASCII<_screenWidth, _screenHeight>& source, ::nwol::SASCIITarget& target)		noexcept	{
	for(uint32_t y = 0, yMax = ::nwol::min(_screenHeight, target.Height()); y < yMax; ++y)
		for(uint32_t x = 0, xMax = ::nwol::min(_screenWidth, target.Width()); x < xMax; ++x)
			target.Text[y][x]							= source.Cells[y][x];
}

				int32_t						render							(::SApplication& instanceApp)																				{
    verbose_printf("%s", "Enter.");
	static constexpr const uint32_t					screenWidth						= ttt::TicTacToe::SCREEN_WIDTH + 1;
	static constexpr const uint32_t					screenHeight					= ttt::TicTacToe::SCREEN_HEIGHT;
	ttt::ScreenASCII<screenWidth, screenHeight>		targetScreenTTT					= {};

	const ::ttt::TicTacToe							& game							= instanceApp.Game;
	const ::ttt::TicTacToeBoard32					& gameBoard						= game.Board;

	if(gameBoard.Winner || !gameBoard.MovesLeft)
		game.DrawResults(gameBoard.GetWinner(), {screenWidth >> 1, (screenHeight >> 1) + 2}, targetScreenTTT.Cells);

	game.DrawBoard({1, 1}, targetScreenTTT.Cells);
	game.DrawBoard(::ttt::CELL_VALUE_X, {1, 5}, targetScreenTTT.Cells);
	game.DrawBoard(::ttt::CELL_VALUE_O, {1, 9}, targetScreenTTT.Cells);

	::nwol::clearASCIIBackBuffer(' ', COLOR_WHITE);

	::nwol::SASCIITarget							actualScreen					= {};
	::nwol::error_t									errMy							= ::nwol::getASCIIBackBuffer(actualScreen);	
	reterr_error_if_errored(errMy, "%s", "Failed to get ASCII target!");
	bltASCIIScreen(targetScreenTTT, actualScreen);

	::nwol::renderGUIASCII(actualScreen, instanceApp.GUI);

	::nwol::presentASCIIBackBuffer();
	
    verbose_printf("%s", "Exit.");
	return 0; 
}
