#include <cstdint>
#include <random>

#ifndef __TICTACTOE_H__9283740923874__
#define __TICTACTOE_H__9283740923874__

namespace ttt 
{
#pragma pack( push, 1 )
	enum CELL_VALUE : uint16_t
		{	CELL_VALUE_EMPTY	= 0
		,	CELL_VALUE_O		= 1
		,	CELL_VALUE_X		= 2
		,	CELL_VALUE_MASK		= 3
		};	// enum

	struct CellCoord {
							uint8_t										Row			: 4;
							uint8_t										Column		: 4;
	};	// struct

	struct CellPick {
							CELL_VALUE									Value		: 2;
							uint16_t									IndexCell	: 4;
							uint16_t									Row			: 2;
							uint16_t									Column		: 2;
							uint16_t									IndexPlayer	: 1;
		//
		inline constexpr												CellPick				(const CellPick&)																					noexcept	= default;
		inline constexpr												CellPick				()																									noexcept	: Value(CELL_VALUE_EMPTY)	, IndexCell(0)		, Row(0)		, Column(0)			, IndexPlayer(0)	{}
		inline constexpr												CellPick				(CELL_VALUE value, uint8_t index)																	noexcept	: Value(value)				, IndexCell(index)	, Row(index%3)	, Column(index/3)	, IndexPlayer(0)	{}
	};	// struct

	struct TicTacToeBoard16 {
							uint16_t									Cells																																	= 0;
		// methods
		inline				bool										GetCell					(CellCoord coord)																			const	noexcept	{ return GetCell(coord.Column*3+coord.Row);						}
		inline				bool										SetCell					(CellCoord coord, bool value)																		noexcept	{ return SetCell(coord.Column*3+coord.Row, value);				}
		inline				bool										GetCell					(int index)																					const	noexcept	{ return (Cells & (1 << index)) ? true : false;					}
							bool										SetCell					(int index, bool value)																				noexcept	{ Cells = Cells | ((value ? 1 : 0) << index);	return value;	}
							bool										EvaluateLines			()																							const				{
			for(int rowOrCol=0; rowOrCol<3; ++rowOrCol) {
						uint16_t													cellMaskRow				= (uint16_t)(0x7UL	<< (rowOrCol*3));
						uint16_t													cellMaskColumn			= (uint16_t)(0x49UL	<< (rowOrCol*1));
						uint16_t 
				check = cellMaskRow		& Cells; if( check == cellMaskRow		)	return true;
				check = cellMaskColumn	& Cells; if( check == cellMaskColumn	)	return true;
			}

			static	const uint16_t												diagonalMask1			= 0x111;
			static	const uint16_t												diagonalMask2			= 0x54;
					const bool													diagonal1				= (diagonalMask1 & Cells) == diagonalMask1;
					const bool													diagonal2				= (diagonalMask2 & Cells) == diagonalMask2;

			return diagonal1 || diagonal2;
		}																											
	};	// struct

	struct TicTacToeBoard32 {
							uint32_t									Cells					= 0;
		//
		inline	constexpr	CELL_VALUE									GetCellValue			(const int row, const int column)															const	noexcept	{	return GetCellValue(column*3+row);											}
		inline				CELL_VALUE									SetCellValue			(const int row, const int column, const CELL_VALUE value)											noexcept	{	return SetCellValue(column*3+row, value);									}
		inline	constexpr	CELL_VALUE									GetCellValue			(const int index)																			const	noexcept	{	return (CELL_VALUE)((Cells & (CELL_VALUE_MASK << (index*2))) >> index*2);	}
		inline				CELL_VALUE									SetCellValue			(const int index, const CELL_VALUE value)															noexcept	{
					CELL_VALUE													currentValue			= GetCellValue(index);
			if(currentValue)
				return currentValue;
			currentValue													= (CELL_VALUE)(value & CELL_VALUE_MASK);
			Cells															= Cells | (currentValue << (index*2));
			return currentValue;
		}
		//
		inline				TicTacToeBoard16							GetCells				(const CELL_VALUE value)																	const	noexcept	{
					TicTacToeBoard16											result;
			for(int y=0; y<3; ++y)
				for(int x=0; x<3; ++x) {
						int32_t															indexLinear				= y*3+x;
						CELL_VALUE														currentValue			= GetCellValue(indexLinear);
					result.SetCell(indexLinear, (currentValue == value) ? true : false);
				}
			return result;
		}
	};	// struct

	template <size_t _Width, size_t _Height>
	struct ScreenASCII {
		static constexpr	const int									Width					= (int)_Width;
		static constexpr	const int									Height					= (int)_Height;
							char										Cells[_Height][_Width]	= {};
	};

	enum PLAYER_CONTROL : uint8_t
		{	PLAYER_CONTROL_HUMAN
		,	PLAYER_CONTROL_AI
		};

	struct TicTacToe
	{
		static constexpr	const uint16_t								SCREEN_WIDTH			= 40;
		static constexpr	const uint16_t								SCREEN_HEIGHT			= 12;
		static constexpr	const uint32_t								Symbols					= '-' | (((uint32_t)'O')<<8) | (((uint32_t)'X')<<16);

							TicTacToeBoard32							Board					= {};
							CELL_VALUE									Winner					= CELL_VALUE_EMPTY;
							uint8_t										MovesLeft				= 9;
							uint8_t										PlayerIndex				= 0;
							PLAYER_CONTROL								PlayerControls[2]		= {PLAYER_CONTROL_HUMAN, PLAYER_CONTROL_AI};
							ScreenASCII<SCREEN_WIDTH+1, SCREEN_HEIGHT>	Screen;
		//
							int											Setup					()																									noexcept	{ Restart();							}
							uint8_t										TurnChange				()																									noexcept	{ return (++PlayerIndex) %= 2;			}
		//
							void										Restart					()																												{ 
			Winner															= CELL_VALUE_EMPTY;	
			MovesLeft														= 9;
			Board															= TicTacToeBoard32();
		}
		// Returns useful information about the move
							CellPick									TurnPlay				(uint8_t cellIndex)																					noexcept	{
					CELL_VALUE												result_value				= CELL_VALUE_EMPTY;
					int														playerIndex					= PlayerIndex;

			// Handle turn depending on the player type. Currently there is no AI system other than random behavior which from time to time makes it look like it knows what she's doing.
			switch(PlayerControls[playerIndex])	{	
			case PLAYER_CONTROL_HUMAN:	//		If it's the human's turn
				if(!Board.GetCellValue(cellIndex))
					result_value												= PlayerIndex ? CELL_VALUE_X : CELL_VALUE_O;
				break;
			default:					//		Otherwise just play any empty cell
				do 
					cellIndex														= rand()%9;
				while(Board.GetCellValue(cellIndex));

				result_value													= PlayerIndex ? CELL_VALUE_X : CELL_VALUE_O;
				break;
			}

			if(result_value) {
				Board.SetCellValue(cellIndex, result_value);
				TurnChange();
				--MovesLeft;
			}
					CellPick													result_pick					= {result_value, cellIndex};
			result_pick.IndexPlayer											= playerIndex;
			return result_pick;
		}
		// Display match results text 
		template <size_t _Width, size_t _Height>
							void										DrawResults				(CELL_VALUE winner, uint32_t x, uint32_t y, char (&screen)[_Height][_Width])				const			{
					char														text[28];
					int32_t														len;
			//
			if(winner)	len														= (int32_t)sprintf_s(text, "Player %u won the match!", (uint32_t)winner);
			else		len														= (int32_t)sprintf_s(text, "Tie!");

			memcpy(&screen[y][x-(len>>1)], text, len);
		}
		//	Display the board
		template <size_t _Width, size_t _Height>
							void										DrawBoard				(uint32_t offsetx, uint32_t offsety, char (&screen)[_Height][_Width])						const			{
			static	const char													* symbols				= (const char*)&Symbols;
			for(uint8_t y=0; y<3; ++y) {
				for(uint8_t x=0; x<3; ++x) {
					screen[offsety+y][offsetx+x]									= symbols[Board.GetCellValue(x, y)];
				}
				screen[offsety+y][_Width-1] = '\n';
			}
			screen[_Height-1][_Width-1] = 0;
		}
		//	Display the board for a given team
		template <size_t _Width, size_t _Height>
							void										DrawBoard				(CELL_VALUE cellValue, uint32_t offsetx, uint32_t offsety, char (&screen)[_Height][_Width])	const			{
					TicTacToeBoard16											board					= Board.GetCells(cellValue);
			static	const char													* symbols				= (const char*)&Symbols;
			for(uint8_t y=0; y<3; ++y) {
				for(uint8_t x=0; x<3; ++x)
					screen[offsety+y][offsetx+x]									= symbols[board.GetCell({x, y}) ? cellValue : 0];

				screen[offsety+y][_Width-1] = '\n';
			}
			screen[_Height-1][_Width-1] = 0;
		}	
		// Execute loop. This isn't really working because it doesn't allow for player input other than from AI.
							CELL_VALUE									Run						()																											{ 
			static	const uint32_t												halfWidth				= Screen.Width	>>1;
			static	const uint32_t												halfHeight				= Screen.Height	>>1;
			PlayerControls[0]												= 
			PlayerControls[1]												= PLAYER_CONTROL_AI;
			while(CELL_VALUE_EMPTY == Winner) {
				Tick((uint8_t)~(uint8_t)0);
				DrawBoard(halfWidth-1, halfHeight-1, Screen.Cells);	
			}
			DrawResults(Winner, halfWidth, halfHeight, Screen.Cells);		//		Congratulate the winner or declare a tie
			return Winner;
		}
		// Play turn and evaluate board. If the turn has to be played by the AI, cellIndex isn't used. Otherwise it determines the cell chosen by the player.
		inline				CellPick									Tick					(uint8_t cellIndex)																							{
					const CellPick												result					= TurnPlay(cellIndex);
			Winner															= EvaluateBoard();
			return result;
		}
		// returns true if a full line was found
		inline				CELL_VALUE									EvaluateBoard			()																							const			{
					const TicTacToeBoard16										boardO					= Board.GetCells(CELL_VALUE_O);
					const TicTacToeBoard16										boardX					= Board.GetCells(CELL_VALUE_X);
				 if(boardO.EvaluateLines())	return CELL_VALUE_O;
			else if(boardX.EvaluateLines())	return CELL_VALUE_X;
			else							return CELL_VALUE_EMPTY;
		}
	};	// struct

#pragma pack( pop )
} // namespace

#endif // __TICTACTOE_H__9283740923874__
