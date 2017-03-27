// Tic Tac Toe implementation which stores its whole state in just 4 bytes.
// Pablo Ariel Zorrilla Cepeda (c) 2017 - Distributed under the MIT License.
#include <cstdint>
#include <random>

#ifndef TICTACTOE_H__9283740923874
#define TICTACTOE_H__9283740923874

namespace ttt 
{
#pragma pack( push, 1 )
	enum CELL_VALUE : uint16_t
		{	CELL_VALUE_EMPTY	= 0
		,	CELL_VALUE_X		= 1
		,	CELL_VALUE_O		= 2
		,	CELL_VALUE_MASK		= 3
		};	// enum

	struct CellCoord {
							uint8_t										Row							: 4;
							uint8_t										Column						: 4;
	};	// struct

	struct CellPick {
							CELL_VALUE									Value						: 2;
							uint16_t									IndexCell					: 4;
							uint16_t									Row							: 2;	// Row		= IndexCell % 3
							uint16_t									Column						: 2;	// Column	= IndexCell / 3
							uint16_t									IndexPlayer					: 1;
		//
		inline constexpr												CellPick					()																									noexcept	: Value(CELL_VALUE_EMPTY)	, IndexCell(0)		, Row(0)		, Column(0)			, IndexPlayer(0)		{}
		inline constexpr												CellPick					(CELL_VALUE value, uint8_t cell, uint8_t player)													noexcept	: Value(value)				, IndexCell(cell)	, Row(cell % 3)	, Column(cell / 3)	, IndexPlayer(player)	{}
	};	// struct

	struct TicTacToeBoard16 {
							uint16_t									Cells						: 9;
							uint16_t									Used						: 4;	// Keep track of the amount of cells used.

		inline constexpr												TicTacToeBoard16			()																									noexcept	: Cells(0), Used(0)									{}
		// methods
		inline constexpr	bool										GetCell						(CellCoord coord)																			const	noexcept	{ return GetCell(coord.Column * 3 + coord.Row);			}
		inline				void										SetCell						(CellCoord coord, bool value)																		noexcept	{ SetCell		(coord.Column * 3 + coord.Row, value);	}
		inline constexpr	bool										GetCell						(int index)																					const	noexcept	{ return (Cells & (1 << index)) ? true : false;		}
							void										SetCell						(int index, bool value)																				noexcept	{ Cells |= ((value ? 1 : 0) << index);				}
							bool										EvaluateLines				()																							const	noexcept	{
			for(int rowOrCol=0; rowOrCol<3; ++rowOrCol) {
						const uint16_t												cellMaskRow					= (uint16_t)(0x7UL	<< (rowOrCol * 3));
						const uint16_t												cellMaskColumn				= (uint16_t)(0x49UL	<< (rowOrCol * 1));
						uint16_t													check;
				if((check = cellMaskRow		& Cells) == cellMaskRow		)	return true;
				if((check = cellMaskColumn	& Cells) == cellMaskColumn	)	return true;
			}

			static constexpr	const uint16_t									diagonalMask1				= 0x111;
			static constexpr	const uint16_t									diagonalMask2				= 0x54;
								const bool										diagonal1					= (diagonalMask1 & Cells) == diagonalMask1;
								const bool										diagonal2					= (diagonalMask2 & Cells) == diagonalMask2;

			return diagonal1 || diagonal2;
		}																											
	};	// struct

	enum PLAYER_CONTROL : uint8_t
		{	PLAYER_CONTROL_AI		= 0
		,	PLAYER_CONTROL_HUMAN	= 1
		};

	// This structure contains the whole game state.
	struct TicTacToeBoard32 {
							uint32_t									Cells						: 18;
							uint32_t									MovesLeft					: 4;
							uint32_t									PlayerIndex					: 1;
							uint32_t									Winner						: 2;
							uint32_t									PlayerControls				: 2;
		//				
		inline constexpr												TicTacToeBoard32			()																									noexcept	: Cells(0), MovesLeft(9), PlayerIndex(0), Winner(CELL_VALUE_EMPTY), PlayerControls(0x1)	{}
		//
		inline constexpr	CELL_VALUE									GetWinner					()																							const	noexcept	{ return (CELL_VALUE)Winner;															}
		inline constexpr	PLAYER_CONTROL								GetPlayerControl			(const int playerIndex)																		const	noexcept	{ return (PLAYER_CONTROL)((PlayerControls & (1 << playerIndex)) >> playerIndex);		}
		inline constexpr	PLAYER_CONTROL								GetPlayerControlCurrent		()																							const	noexcept	{ return GetPlayerControl(PlayerIndex);													}
		inline constexpr	CELL_VALUE									GetCellValue				(const int row, const int column)															const	noexcept	{ return GetCellValue	(column * 3 + row);												}
		inline				void										SetCellValue				(const int row, const int column, const CELL_VALUE value)											noexcept	{ SetCellValue			(column * 3 + row, value);										}
		inline constexpr	CELL_VALUE									GetCellValue				(const int index)																			const	noexcept	{ return (CELL_VALUE)((Cells & (CELL_VALUE_MASK << (index * 2))) >> index * 2);			}
		inline				void										SetCellValue				(const int index, const CELL_VALUE value)															noexcept	{
					const CELL_VALUE											currentValue				= (CELL_VALUE)(value & CELL_VALUE_MASK);
			Cells															= Cells | (currentValue << (index * 2));
		}
		inline				TicTacToeBoard16							GetCells					(const CELL_VALUE value)																	const	noexcept	{
					TicTacToeBoard16											result;
			for(int y = 0; y < 3; ++y)
				for(int x = 0; x < 3; ++x) {
							const int32_t												indexLinear					= y * 3 + x;
							const CELL_VALUE											currentValue				= GetCellValue(indexLinear);
					if(currentValue == value) {
						result.SetCell(indexLinear, true);
						++result.Used;
					}
				}
			return result;
		}
	};	// struct

	template <size_t _Width, size_t _Height>
	struct ScreenASCII {
		static constexpr	const int									Width						= (int)_Width;
		static constexpr	const int									Height						= (int)_Height;
							char										Cells[_Height][_Width]		= {};
	};

	struct TicTacToe {
		static constexpr	const uint16_t								SCREEN_WIDTH				= 40;
		static constexpr	const uint16_t								SCREEN_HEIGHT				= 12;
		static constexpr	const char									Symbols[]					= {'-', 'X', 'O'};

							TicTacToeBoard32							Board						= {};

							uint8_t										TurnChange					()																									noexcept	{ return (++Board.PlayerIndex) %= 2;	}
							void										Restart						()																									noexcept	{ 
			Board.Winner													= CELL_VALUE_EMPTY;	
			const int															playerIndex					= Board.PlayerIndex;
			Board															= TicTacToeBoard32();
			Board.PlayerIndex												= playerIndex;
		}
		// Returns useful information about the move
							CellPick									TurnPlay					(uint8_t cellIndex)																					noexcept	{
					CELL_VALUE													result_value				= CELL_VALUE_EMPTY;
					const uint8_t												playerIndex					= Board.PlayerIndex;

			// Handle turn depending on the player type. Currently there is no AI system other than random behavior which from time to time makes it look like it knows what she's doing.
			switch(Board.GetPlayerControl(playerIndex))	{	
			case PLAYER_CONTROL_HUMAN:	//		If it's the human's turn
				if(!Board.GetCellValue(cellIndex))
					result_value													= Board.PlayerIndex ? CELL_VALUE_O : CELL_VALUE_X;
				break;
			default:					//		Otherwise just play any empty cell
				do cellIndex														= rand()%9;
				while(Board.GetCellValue(cellIndex));

				result_value													= Board.PlayerIndex ? CELL_VALUE_O : CELL_VALUE_X;
				break;
			}

			if(CELL_VALUE_EMPTY != result_value) {
				Board.SetCellValue(cellIndex, result_value);
				TurnChange();
				--Board.MovesLeft;
			}
			return {result_value, cellIndex, playerIndex};
		}
		// returns true if a full line was found
		inline				CELL_VALUE									EvaluateBoard				()																							const	noexcept	{
					const TicTacToeBoard16										boardX						= Board.GetCells(CELL_VALUE_X);
					const TicTacToeBoard16										boardO						= Board.GetCells(CELL_VALUE_O);
				 if(boardX.EvaluateLines())	return CELL_VALUE_X;
			else if(boardO.EvaluateLines())	return CELL_VALUE_O;
			else 							return CELL_VALUE_EMPTY;
		}
		// Play turn and evaluate board. If the turn has to be played by the AI, cellIndex isn't used. Otherwise it determines the cell chosen by the player.
		inline				CellPick									Tick						(uint8_t cellIndex)																					noexcept	{
					const CellPick												result						= TurnPlay(cellIndex);
			Board.Winner													= EvaluateBoard();
			return result;
		}
		// Execute loop. This isn't really working but serves as example of what your application should do more or less depending on user input.
							CELL_VALUE									Run							()																									noexcept	{ 
					ScreenASCII<SCREEN_WIDTH+1, SCREEN_HEIGHT>					targetScreenTTT;
			static	const uint32_t												halfWidth					= targetScreenTTT.Width		>> 1;
			static	const uint32_t												halfHeight					= targetScreenTTT.Height	>> 1;
			Board.PlayerControls											= (PLAYER_CONTROL_AI << 1) | PLAYER_CONTROL_AI;
			static	const uint32_t												cellIndex					= 0;
			while(CELL_VALUE_EMPTY == Board.Winner) {
				Tick(cellIndex);
				DrawBoard(halfWidth - 1, halfHeight - 1, targetScreenTTT.Cells);	
			}
			DrawResults(Board.GetWinner(), halfWidth, halfHeight, targetScreenTTT.Cells);		//		Congratulate the winner or declare a tie
			return Board.GetWinner();
		}
		// Display match results text 
		template <size_t _Width, size_t _Height>
							void										DrawResults					(CELL_VALUE winner, uint32_t x, uint32_t y, char (&screen)[_Height][_Width])				const	noexcept	{
					char														text[25];
					int32_t														len;
			//
			if(winner)	len														= (int32_t)sprintf_s(text, "Player %u won the match!", (uint32_t)winner);
			else		len														= (int32_t)sprintf_s(text, "Tie!");

			memcpy(&screen[y][x - (len >> 1)], text, len);
		}
		//	Display the board
		template <size_t _Width, size_t _Height>
							void										DrawBoard					(uint32_t offsetx, uint32_t offsety, char (&screen)[_Height][_Width])						const	noexcept	{
			for(uint8_t y = 0, yMax = (_Height < 3) ? _Height : 3; y < yMax; ++y) {
				for(uint8_t x = 0, xMax = (_Width < 3) ? _Width : 3; x < xMax; ++x)
					screen[offsety + y][offsetx + x]									= Symbols[Board.GetCellValue(x, y)];
				screen[offsety + y][_Width - 1]										= '\n';
			}
			screen[_Height - 1][_Width - 1]										= 0;
		}
		//	Display the board for a given team
		template <size_t _Width, size_t _Height>
							void										DrawBoard					(CELL_VALUE cellValue, uint32_t offsetx, uint32_t offsety, char (&screen)[_Height][_Width])	const	noexcept	{
					const TicTacToeBoard16										board						= Board.GetCells(cellValue);
			for(uint8_t y = 0, yMax = (_Height < 3) ? _Height : 3; y < yMax; ++y) {
				for(uint8_t x = 0, xMax = (_Width < 3) ? _Width : 3; x < xMax; ++x)
					screen[offsety + y][offsetx + x]									= Symbols[board.GetCell({x, y}) ? cellValue : 0];
				screen[offsety + y][_Width - 1]										= '\n';
			}
			if((offsety + 2) < _Height && (offsetx + 4) < _Width)	// print number of cells used if the screen is large enough
				screen[offsety + 2][offsetx + 4]									= (char)('0' + board.Used);
			screen[_Height - 1][_Width - 1]										= 0;
		}	
	};	// struct
#pragma pack( pop )
} // namespace

#endif // TICTACTOE_H__9283740923874
