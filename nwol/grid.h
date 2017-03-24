#include "grid_view.h"

#include "text.h"
#include "noise.h"

#ifndef __TILE_H__973098721983761298736129873691872361__
#define __TILE_H__973098721983761298736129873691872361__

namespace nwol
{
#pragma pack(push, 1)

	template<typename _T, size_t _Width, size_t _Depth>
	struct SGrid : public grid_view<_T>
	{
		typedef					_T						TCell;
		TCell					Cells[_Depth][_Width]	= {};

		static const uint32_t	Width					= (uint32_t)_Width;
		static const uint32_t	Depth					= (uint32_t)_Depth;

		inline constexpr		SGrid					()					: grid_view(&Cells[0][0], _Width, _Depth) {}
	};

	template<typename _CellType, size_t _Width, size_t _Height>
	void						fillCellsFromNoise		(SGrid<_CellType, _Width, _Height>& grid, const _CellType& value, int64_t seed, const _CellType& clearValue = ' ', int32_t diceFaces=10) {
		_CellType						* cells					= &grid.Cells[0][0];
		for(uint32_t i=0, count=_Width*_Height;  i<count; ++i) {
			double							noise					= nwol::noiseNormal1D(i+1, seed);
			int32_t							dice					= int32_t(noise * diceFaces);
			if(0 == dice)
				cells[i]					= value;
		}
	}

	template<typename _CellType, size_t _Width, size_t _Height>
	void						clearGrid				( SGrid<_CellType, _Width, _Height>& grid, const _CellType& clearValue = ' ') {
		_CellType						* cells					= &grid.Cells[0][0];
		for(uint32_t i=0, count=_Width*_Height; i<count; ++i )
			cells[i]					= clearValue;
	}

	template<typename _CellType, size_t _Width, size_t _Height>
	void						drawGridBorders			( SGrid<_CellType, _Width, _Height>& display, const _CellType& value) {
		for(uint32_t z=0, maxZ=_Height; z<maxZ; ++z){
			display[z][0]				= value; 
			display[z][_Width-1]		= value; 
		}
		for(uint32_t x=0, maxX=_Width ; x<maxX; ++x){ 
			display[0][x]				= value; 
			display[_Height-1][x]		= value; 
		}
	}

	template<typename _TCell, size_t _Width, size_t _Height>
	int32_t						valueToGrid				( SGrid<_TCell, _Width, _Height>& grid, int32_t offsetLine, int32_t offsetColumn, nwol::ALIGN_SCREEN align, const _TCell* values, int32_t valueCount, int32_t repeat=0 ) {
		return valueToRect(&grid.Cells[0][0], _Width, _Height, offsetLine, offsetColumn, align, values, valueCount, repeat);
	}

	template<size_t _Width, size_t _Height>
	int32_t						lineToGrid				( SGrid<char, _Width, _Height>& display, int32_t offsetLine, int32_t offsetColumn, nwol::ALIGN_SCREEN align, const char* text, uint32_t charCount = 0xFFFFFFFF ) {
		return lineToRect(&display.Cells[0][0], _Width, _Height, offsetLine, offsetColumn, align, text, charCount);
	}

	template<size_t _Width, size_t _Height>
	int32_t						lineToGridColored		( SGrid<char, _Width, _Height>& display, SGrid<uint16_t, _Width, _Height>& textAttributes, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, nwol::ALIGN_SCREEN align, const char* text, uint32_t charCount = 0xFFFFFFFF ) {
		int32_t							actualX					= lineToRect(&display.Cells[0][0], _Width, _Height, offsetLine, offsetColumn, align, text, charCount);
		valueToGrid(textAttributes, offsetLine, actualX, nwol::SCREEN_LEFT, &messageColor, 1, (int32_t)std::min((uint32_t)strlen(text), charCount));
		return actualX;
	}

	template<typename _CellType, size_t _Width, size_t _Height, typename... _Args>
	int32_t						printfToGrid			( SGrid<_CellType, _Width, _Height>& display, int32_t offsetLine, int32_t offsetColumn, nwol::ALIGN_SCREEN align, const char* format, _Args&&... args ) {
		int32_t							actualX					= printfToRect(&display.Cells[0][0], _Width, _Height, offsetLine, offsetColumn, align, format, args...);
		return actualX;
	}

	template<typename _CellType, size_t _Width, size_t _Height, typename... _Args>
	int32_t						printfToGridColored( SGrid<_CellType, _Width, _Height>& display, SGrid<uint16_t, _Width, _Height>& textAttributes, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, nwol::ALIGN_SCREEN align, const char* format, _Args&&... args ) {
		char							precookStr[1024]		= {};
		int32_t							precookLen				= sprintf_s(precookStr, format, args...);
		int32_t							actualX					= lineToGridColored(display, textAttributes, messageColor, offsetLine, offsetColumn, align, precookStr);
		return actualX;
	}

#pragma pack(pop)

} // namespace

#endif // __TILE_H__973098721983761298736129873691872361__