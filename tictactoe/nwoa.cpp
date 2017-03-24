#include "nwoa.h"

#include "text.h"
#include "ascii_screen.h"
#include "gui.h"

#include "runtime_impl.h"
#include "nwol_debug.h"

#include <thread>
#include <process.h>
#include <ctime>

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(SApplication, "Tic Tac Toe", 0, 1);

int32_t										cleanup							(::SApplication& instanceApp)							{ 
	::nwol::shutdownASCIIScreen();

	::networkDisable(instanceApp);

	return 0; 
}

int32_t										setup							(::SApplication& instanceApp)							{ 
	srand((unsigned int)time(0));

	::nwol::SGUI									& guiSystem						= instanceApp.GUI;
	int32_t											screenWidth						= guiSystem.TargetSizeASCII.x;
	int32_t											screenHeight					= guiSystem.TargetSizeASCII.y;
	::nwol::initASCIIScreen(screenWidth, screenHeight);
	::nwol::setASCIIScreenTitle(appTitle());
	::networkEnable(instanceApp);

	// Create exit button
	::nwol::glabel									newControlLabel					= "Exit";
	::nwol::SGUIControl								newControl;
	newControl.AreaASCII						= {{1, 1}, {(int32_t)newControlLabel.size(), 1}}	;
	newControl.Text								= newControlLabel									;
	::nwol::error_t									errMy							= ::nwol::createControl(instanceApp.GUI, newControl);
	reterr_msg_if_error(errMy, "%s: \"%s\".", "Failed to create control", newControlLabel.begin());

	newControl.AlignArea						= (::nwol::ALIGN_SCREEN)(::nwol::SCREEN_CENTER | ::nwol::SCREEN_VCENTER)	;
	for(int32_t y=0; y<3; ++y) 
		for(int32_t x=0; x<3; ++x) {
			newControl.AreaASCII						= {{-3+x*3, -3+y}, {3, 1}}								;
			newControl.Text								= ::nwol::glabel::statics().space						;
			::nwol::SControlTextColorASCII					& colorsConsole				= newControl.TextColorsASCII;
			colorsConsole.Color.Background				= ((y*3+x) % 2) ? COLOR_DARKGREY : COLOR_WHITE			;
			colorsConsole.Color.Foreground				= COLOR_YELLOW											;
			colorsConsole.ColorPressed					= {COLOR_DARKGREY, COLOR_YELLOW}						;
			//newControl.AlignText						= ::nwol::SCREEN_CENTER									; this is not working yet
			::nwol::createControl(instanceApp.GUI, newControl);
		}

	newControlLabel								= "Restart"																;
	newControl									= ::nwol::SGUIControl()													;
	newControl.AlignArea						= (::nwol::ALIGN_SCREEN)(::nwol::SCREEN_CENTER | ::nwol::SCREEN_BOTTOM)	;
	newControl.AreaASCII						= {{0, 1}, {(int32_t)newControlLabel.size(), 1}}						;
	newControl.Text								= newControlLabel														;
	::nwol::createControl(instanceApp.GUI, newControl);

	return 0; 
}

#if defined(__WINDOWS__)
#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif
#else 
#error "Unsupported platform. Define key code for this platform in order to continue."
#endif

int32_t										update							(::SApplication& instanceApp, bool exitRequested)		{
	if(exitRequested)
		return ::nwol::APPLICATION_STATE_EXIT;

	// Update subsystems
	::nwol::SInput									& inputSystem					= instanceApp.Input;	::nwol::pollInput(inputSystem);
	::nwol::SGUI									& guiSystem						= instanceApp.GUI;		::nwol::updateGUI(guiSystem, inputSystem);

	// Update game
	::ttt::TicTacToe								& game							= instanceApp.Game;
	static const ::nwol::glabel						symbols[]						= {" O ", " X "};
	::nwol::array_pod<::nwol::CONTROL_FLAG>			& controlFlags					= guiSystem.Controls.ControlFlags;

	const ::ttt::PLAYER_CONTROL						& currentPlayer					= game.PlayerControls[game.PlayerIndex];
	::ttt::CellPick									cellPicked;
	static uint8_t									tickDelay						= 0;
	for(uint32_t iControl = 0, controlCount = controlFlags.size(); iControl < controlCount; ++iControl)
		if(::nwol::bit_true(controlFlags[iControl], ::nwol::CONTROL_FLAG_EXECUTE)) {
			debug_printf("Execute %u.", iControl);
			switch(iControl) {
			case 0:		return ::nwol::APPLICATION_STATE_EXIT; 
			case 10:
				game.Restart();

				for(int32_t y=0; y<3; ++y) {		// Clear board controls.
					for(int32_t x=0; x<3; ++x){
						int										controlIndex						= (y*3+x)+1;
						guiSystem.Controls.Text					[controlIndex]						= ::nwol::glabel::statics().space;
						guiSystem.Controls.TextColorsASCII		[controlIndex].Color.Foreground		= COLOR_YELLOW;
						guiSystem.Controls.TextColorsASCII		[controlIndex].ColorPressed			= {COLOR_DARKGREY, COLOR_YELLOW};
					}
				}
				debug_print("Game restarted.");
				break;

			default: 
				if(currentPlayer == ::ttt::PLAYER_CONTROL_HUMAN && game.MovesLeft && !game.Winner) {
					uint8_t											cellIndex			= (uint8_t)(iControl-1);
					cellPicked									= {::ttt::CELL_VALUE_EMPTY, cellIndex};
					if(::ttt::CELL_VALUE_EMPTY == instanceApp.Game.Board.GetCellValue(cellIndex)) {
						cellPicked									= instanceApp.Game.Tick(cellIndex);
						tickDelay									= 2;
					}
					int32_t											playerNumber			= cellPicked.IndexPlayer+1;
					const char										* textAction			= cellPicked.Value ? "Player %u played {%u, %u}" : "Player %u cannot move to {%u, %u}";
					debug_printf(textAction, playerNumber, cellPicked.Row, cellPicked.Column);
				}
				break;
			}
		}

	if(currentPlayer != ::ttt::PLAYER_CONTROL_HUMAN && game.MovesLeft && !game.Winner && !tickDelay && !cellPicked.Value)
		cellPicked									= game.Tick((uint8_t)~(uint8_t)0);

	if(cellPicked.Value) {
		uint32_t									controlIndex							= cellPicked.IndexCell+1;
		guiSystem.Controls.TextColorsASCII			[controlIndex].Color		.Foreground	= 
		guiSystem.Controls.TextColorsASCII			[controlIndex].ColorPressed	.Background	= cellPicked.IndexPlayer ? COLOR_RED : COLOR_BLUE;
		guiSystem.Controls.Text						[controlIndex]							= symbols[cellPicked.IndexPlayer];
	}

	tickDelay += 2;

	return 0; 
}

int32_t										render							(::SApplication& instanceApp)							{ 
	::nwol::clearASCIIBackBuffer(' ', COLOR_WHITE);

	::nwol::SASCIITarget							target;
	::nwol::error_t									errMy							= ::nwol::getASCIIBackBuffer(target);
	reterr_error_if_errored(errMy, "%s", "Failed to get ASCII target!");

	static const uint32_t							screenWidth						= instanceApp.Game.Screen.Width;
	static const uint32_t							screenHeight					= instanceApp.Game.Screen.Height;

	::ttt::ScreenASCII<screenWidth, screenHeight>	& screen						= instanceApp.Game.Screen;
	memset(screen.Cells[0], ' ', ::nwol::size(screen.Cells));

	::ttt::TicTacToe								& game							= instanceApp.Game;

	if(game.Winner || !game.MovesLeft)
		instanceApp.Game.DrawResults(game.Winner, screenWidth>>1, (screenHeight>>1)+2, screen.Cells);

	instanceApp.Game.DrawBoard(1, 1, screen.Cells);
	instanceApp.Game.DrawBoard(::ttt::CELL_VALUE_O, 1, 5, screen.Cells);
	instanceApp.Game.DrawBoard(::ttt::CELL_VALUE_X, 1, 9, screen.Cells);

	for(uint32_t y = 0, yMax = ::nwol::min(screenHeight, target.Height()); y<yMax; ++y)
		for(uint32_t x = 0, xMax = ::nwol::min(screenWidth, target.Width()); x<xMax; ++x) {
			uint32_t										indexLinear						= y * target.Width() + x;
			target.Text.begin()[indexLinear]			= screen.Cells[y][x];
		}

	::nwol::renderGUIASCII(target, instanceApp.GUI);

	::nwol::presentASCIIBackBuffer();
	
	return 0; 
}
