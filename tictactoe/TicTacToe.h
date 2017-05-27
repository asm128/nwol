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

	template<typename _tNumeric> 
	struct Coord2D {
							_tNumeric									x, y;
	};
	typedef					Coord2D<uint32_t>							Coord2Du32;

	// For representing the board cell positions we use this compact structure that works well with grids up to 16x16 cells. For everything else, such as screen coordinates, we use the Coord2D template structure.
	struct CellCoord {	
							uint8_t										Row								: 4;
							uint8_t										Column							: 4;
	};	// struct

	// This structure is only suitable for boards of up to 3x3 cells. It represents the position of a picked cell and the player.
	struct CellPick {
		// ----------------------------------------------------  Variables	-------------------------------------------------------------------------
							CELL_VALUE									Value							: 2;
							uint16_t									IndexCell						: 4;
							uint16_t									Row								: 2;	// Row		= IndexCell % 3
							uint16_t									Column							: 2;	// Column	= IndexCell / 3
							uint16_t									IndexPlayer						: 1;
		// ----------------------------------------------------	  Methods	-------------------------------------------------------------------------
		inline constexpr												CellPick						()																											noexcept	: Value(CELL_VALUE_EMPTY)	, IndexCell(0)		, Row(0)		, Column(0)			, IndexPlayer(0)		{}
		inline constexpr												CellPick						(const CELL_VALUE value, const uint8_t cell, const uint8_t player)											noexcept	: Value(value)				, IndexCell(cell)	, Row(cell % 3)	, Column(cell / 3)	, IndexPlayer(player)	{}
	};	// struct

	// This structure represents a boolean layout of the board. It's used to determine the cells occupied by a given player. 
	struct TicTacToeBoard16 {
		// ----------------------------------------------------  Variables	-------------------------------------------------------------------------
							uint16_t									Cells							: 9;	
							uint16_t									Used							: 4;	// Keep track of the amount of cells set to true.
		// ----------------------------------------------------	  Methods	-------------------------------------------------------------------------
		inline constexpr												TicTacToeBoard16				()																											noexcept	: Cells(0), Used(0)										{}
		// methods
		inline constexpr	bool										GetCell							(const CellCoord coord)																				const	noexcept	{ return GetCell(coord.Column * 3 + coord.Row);			}
		inline				void										SetCell							(const CellCoord coord, const bool value)																	noexcept	{ SetCell		(coord.Column * 3 + coord.Row, value);	}
		inline constexpr	bool										GetCell							(const int index)																					const	noexcept	{ return (Cells & (1 << index)) ? true : false;			}
		inline				void										SetCell							(const int index, const bool value)																			noexcept	{ Cells |= ((value ? 1 : 0) << index);					}
							bool										EvaluateLines					()																									const	noexcept	{
			static constexpr	const uint16_t									maskHorizontal					= 0x7;	// bits representing a row		(111)
			static constexpr	const uint16_t									maskVertical					= 0x49;	// bits representing a column	(001001001)
			static constexpr	const uint16_t									cellMaskRows	[3]				= {(maskHorizontal	<< (0 * 3)), (maskHorizontal	<< (1 * 3)), (maskHorizontal	<< (2 * 3)), };		
			static constexpr	const uint16_t									cellMaskColumns	[3]				= {(maskVertical	<< (0 * 1)), (maskVertical		<< (1 * 1)), (maskVertical		<< (2 * 1)), };		
								uint16_t										check;
			for(int rowOrCol = 0; rowOrCol < 3; ++rowOrCol) {
				if(((check = cellMaskRows		[rowOrCol]) & Cells) == cellMaskRows	[rowOrCol] ) return true;
				if(((check = cellMaskColumns	[rowOrCol]) & Cells) == cellMaskColumns	[rowOrCol] ) return true;
			}
			static constexpr	const uint16_t									diagonalMask1					= 0x111;
			static constexpr	const uint16_t									diagonalMask2					= 0x54;
								const bool										diagonal1						= (diagonalMask1 & Cells) == diagonalMask1;
								const bool										diagonal2						= (diagonalMask2 & Cells) == diagonalMask2;

			return diagonal1 || diagonal2;
		}																												
	};	// struct

	// Allows us to define the type of player 
	enum PLAYER_CONTROL : uint8_t
		{	PLAYER_CONTROL_AI		= 0
		,	PLAYER_CONTROL_HUMAN	= 1
		};

	// This structure contains the whole game state.
	struct TicTacToeBoard32 {
		// ----------------------------------------------------  Variables	-------------------------------------------------------------------------
							uint32_t									Cells							: 18;
							uint32_t									MovesLeft						: 4;
							uint32_t									PlayerIndex						: 1;
							uint32_t									Winner							: 2;
							uint32_t									PlayerControls					: 2;
							uint32_t									Padding							: 5;
		// ----------------------------------------------------	  Methods	-------------------------------------------------------------------------
		inline constexpr												TicTacToeBoard32				()																											noexcept	: Cells(0), MovesLeft(9), PlayerIndex(0), Winner(CELL_VALUE_EMPTY), PlayerControls(0x1), Padding(0)	{}
		//
		inline constexpr	CELL_VALUE									GetWinner						()																									const	noexcept	{ return (CELL_VALUE)Winner;																		}
		inline constexpr	PLAYER_CONTROL								GetPlayerControl				(const int playerIndex)																				const	noexcept	{ return (PLAYER_CONTROL)((PlayerControls & (1 << playerIndex)) >> playerIndex);					}
		inline constexpr	PLAYER_CONTROL								GetPlayerControlCurrent			()																									const	noexcept	{ return GetPlayerControl(PlayerIndex);																}
		inline constexpr	CELL_VALUE									GetCellValue					(const int row, const int column)																	const	noexcept	{ return GetCellValue	(column * 3 + row);															}
		inline				void										SetCellValue					(const int row, const int column, const CELL_VALUE value)													noexcept	{ SetCellValue			(column * 3 + row, value);													}
		inline constexpr	CELL_VALUE									GetCellValue					(const int index)																					const	noexcept	{ return (CELL_VALUE)((Cells & (CELL_VALUE_MASK << (index * 2))) >> index * 2);						}
		inline				void										SetCellValue					(const int index, const CELL_VALUE value)																	noexcept	{ Cells = Cells | ((value & CELL_VALUE_MASK) << (index * 2));										}
							TicTacToeBoard16							GetCells						(const CELL_VALUE value)																			const	noexcept	{
			TicTacToeBoard16													result;
			for(int y = 0; y < 3; ++y)
				for(int x = 0; x < 3; ++x) {
					const int32_t														indexLinear						= y * 3 + x;
					const CELL_VALUE													currentValue					= GetCellValue(indexLinear);
					if(currentValue == value) {
						result.SetCell(indexLinear, true);
						++result.Used;
					}
				}
			return result;
		}
	};	// struct

	template <size_t _sizeWidth, size_t _sizeHeight>
	struct ScreenASCII {
		// ----------------------------------------------------  Constants	-------------------------------------------------------------------------
		static constexpr	const int									Width							= (int)_sizeWidth;
		static constexpr	const int									Height							= (int)_sizeHeight;
							char										Cells[_sizeHeight][_sizeWidth]	= {};
		// ----------------------------------------------------	  Methods	-------------------------------------------------------------------------
		inline constexpr												ScreenASCII						()																											noexcept	= default;
	};

	struct TicTacToe {
		// ----------------------------------------------------  Constants	-------------------------------------------------------------------------
		static constexpr	const uint16_t								SCREEN_WIDTH					= 40;
		static constexpr	const uint16_t								SCREEN_HEIGHT					= 12;
		static constexpr	const char									Symbols[]						= {'-', 'X', 'O'};
		// ----------------------------------------------------  Variables	-------------------------------------------------------------------------
							TicTacToeBoard32							Board							= {};
		// ----------------------------------------------------	  Methods	-------------------------------------------------------------------------
		inline				uint8_t										TurnChange						()																											noexcept	{ return (++Board.PlayerIndex) %= 2;	}
		inline				void										Restart							()																											noexcept	{ 
			const int															playerIndex						= Board.PlayerIndex;
			Board															= {};
			Board.PlayerIndex												= playerIndex;
		}
		// Returns useful information about the move
							CellPick									TurnPlay						(uint8_t cellIndex)																							noexcept	{
			CELL_VALUE															result_value					= CELL_VALUE_EMPTY;
			const uint8_t														playerIndex						= Board.PlayerIndex;

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

			if(CELL_VALUE_EMPTY != result_value) {	// If a valid cell has been played, mark it in the board.
				Board.SetCellValue(cellIndex, result_value);
				TurnChange();
				--Board.MovesLeft;
			}
			return {result_value, cellIndex, playerIndex};
		}
		// Returns true if a full line was found
		inline				CELL_VALUE									EvaluateBoard					()																									const	noexcept	{
			const TicTacToeBoard16												boardX							= Board.GetCells(CELL_VALUE_X); 
			const TicTacToeBoard16												boardO							= Board.GetCells(CELL_VALUE_O); 
			if(boardX.EvaluateLines()) return CELL_VALUE_X;
			if(boardO.EvaluateLines()) return CELL_VALUE_O;
			return CELL_VALUE_EMPTY;
		}
		// Play turn and evaluate board. If the turn has to be played by the AI, cellIndex isn't used. Otherwise it determines the cell chosen by the player.
		inline				CellPick									Tick							(const uint8_t cellIndex)																					noexcept	{
			const CellPick														result							= TurnPlay(cellIndex);
			Board.Winner													= EvaluateBoard();
			return result;
		}
		// Execute loop. This isn't really working but serves as example of what your application should do more or less depending on user input.
							CELL_VALUE									Run								()																											noexcept	{ 
			ScreenASCII<SCREEN_WIDTH+1, SCREEN_HEIGHT>							targetScreenTTT;
			static	const uint32_t												halfWidth						= targetScreenTTT.Width		>> 1;
			static	const uint32_t												halfHeight						= targetScreenTTT.Height	>> 1;
			Board.PlayerControls											= (PLAYER_CONTROL_AI << 1) | PLAYER_CONTROL_AI;
			static	const uint32_t												cellIndex						= 0;	// When the player is a human player, cellIndex should contain the index of the cell played.
			while(CELL_VALUE_EMPTY == Board.Winner) {
				Tick(cellIndex);
				DrawBoard({halfWidth - 1, halfHeight - 1}, targetScreenTTT.Cells);	
			}
			DrawResults(Board.GetWinner(), {halfWidth, halfHeight}, targetScreenTTT.Cells);		//		Congratulate the winner or declare a tie
			return Board.GetWinner();
		}
		// Display match results text 
		template <size_t _sizeWidth, size_t _sizeHeight>
		inline				void										DrawResults						(const CELL_VALUE winner, const Coord2Du32& textCenter, char (&screen)[_sizeHeight][_sizeWidth])	const	noexcept	{
			char																text[25]						= {};
			const int32_t														len								= (int32_t)(winner ? sprintf_s(text, "Player %u won the match!", (uint32_t)winner) : sprintf_s(text, "Tie!"));
			memcpy(&screen[textCenter.y][textCenter.x - (len >> 1)], text, len);
		}
		// Display the board
		template <size_t _sizeWidth, size_t _sizeHeight>
							void										DrawBoard						(const Coord2Du32& offset, char (&screen)[_sizeHeight][_sizeWidth])									const	noexcept	{
			for(uint8_t y = 0, yMax = (_sizeHeight < 3) ? _sizeHeight : 3; y < yMax; ++y) {
				for(uint8_t x = 0, xMax = (_sizeWidth < 3) ? _sizeWidth : 3; x < xMax; ++x)
					screen[offset.y + y][offset.x + x]								= Symbols[Board.GetCellValue(x, y)];
				screen[offset.y + y][_sizeWidth - 1]							= '\n';
			}
			screen[_sizeHeight - 1][_sizeWidth - 1]							= 0;
		}
		// Display the board for a given team
		template <size_t _sizeWidth, size_t _sizeHeight>
							void										DrawBoard						(const CELL_VALUE cellValue, const Coord2Du32& offset, char (&screen)[_sizeHeight][_sizeWidth])		const	noexcept	{
			const TicTacToeBoard16												board							= Board.GetCells(cellValue);
			for(uint8_t y = 0, yMax = (_sizeHeight < 3) ? _sizeHeight : 3; y < yMax; ++y) {
				for(uint8_t x = 0, xMax = (_sizeWidth < 3) ? _sizeWidth : 3; x < xMax; ++x)
					screen[offset.y + y][offset.x + x]								= Symbols[board.GetCell({x, y}) ? cellValue : 0];
				screen[offset.y + y][_sizeWidth - 1]							= '\n';
			}
			if((offset.y + 2) < _sizeHeight && (offset.x + 4) < _sizeWidth)	// print number of cells used if the screen is large enough
				screen[offset.y + 2][offset.x + 4]								= (char)('0' + board.Used);
			screen[_sizeHeight - 1][_sizeWidth - 1]							= 0;
		}	
	};	// struct
#pragma pack( pop )
} // namespace

#endif // TICTACTOE_H__9283740923874
