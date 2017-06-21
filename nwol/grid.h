/// Copyright 2016-2017 - asm128
#include "grid_view.h"

#include "text.h"
#include "nwol_noise.h"

#ifndef __TILE_H__973098721983761298736129873691872361__
#define __TILE_H__973098721983761298736129873691872361__

namespace nwol
{
#pragma pack(push, 1)

	template<typename _tCell, size_t _sizeWidth, size_t _sizeDepth>
	struct SGrid : public grid_view<_tCell> {
							typedef						_tCell							TCell;

		static constexpr	const uint32_t				Width							= (uint32_t)_sizeWidth;
		static constexpr	const uint32_t				Depth							= (uint32_t)_sizeDepth;

							TCell						Cells[_sizeDepth][_sizeWidth]	= {};

		inline constexpr								SGrid							()						: grid_view(&Cells[0][0], _sizeWidth, _sizeDepth)																											{}
	};

	template<typename _tCell, size_t _sizeWidth, size_t _sizeHeight>
	void											fillCellsFromNoise				(SGrid<_tCell, _sizeWidth, _sizeHeight>& grid, const _tCell& value, int64_t seed, const _tCell& clearValue = ' ', int32_t diceFaces=10)												{
		_tCell												* cells							= &grid.Cells[0][0];
		for(uint32_t i = 0, count = _sizeWidth * _sizeHeight;  i < count; ++i) {
			double												noise							= ::nwol::noiseNormal1D(i+1, seed);
			int32_t												dice							= int32_t(noise * diceFaces);
			if(0 == dice)
				cells[i]										= value;
		}
	}

	template<typename _tCell, size_t _sizeWidth, size_t _sizeHeight>
	void											clearGrid						(SGrid<_tCell, _sizeWidth, _sizeHeight>& grid, const _tCell& clearValue = ' ')																										{
		_tCell												* cells							= &grid.Cells[0][0];
		for(uint32_t i = 0, count = _sizeWidth * _sizeHeight; i < count; ++i )
			cells[i]										= clearValue;
	}

	template<typename _tCell, size_t _sizeWidth, size_t _sizeHeight>
	void											drawGridBorders					( ::nwol::SGrid<_tCell, _sizeWidth, _sizeHeight>& display, const _tCell& value)																												{
		for(uint32_t z = 0, maxZ = _sizeHeight; z < maxZ; ++z) {
			display[z][0]									= value; 
			display[z][_sizeWidth-1]						= value; 
		}
		for(uint32_t x=0, maxX=_sizeWidth ; x<maxX; ++x) {
			display[0][x]									= value; 
			display[_sizeHeight-1][x]						= value; 
		}
	}

	template<typename _tCell, size_t _sizeWidth, size_t _sizeHeight>
	int32_t											valueToGrid						( ::nwol::SGrid<_tCell, _sizeWidth, _sizeHeight>& grid, int32_t offsetLine, int32_t offsetColumn, ::nwol::ALIGN_SCREEN align, const _tCell* values, int32_t valueCount, int32_t repeat=0 )	{
		return ::nwol::valueToRect(&grid.Cells[0][0], _sizeWidth, _sizeHeight, offsetLine, offsetColumn, align, values, valueCount, repeat);
	}

	template<size_t _sizeWidth, size_t _sizeHeight>
	int32_t											lineToGrid						( ::nwol::SGrid<char, _sizeWidth, _sizeHeight>& display, int32_t offsetLine, int32_t offsetColumn, ::nwol::ALIGN_SCREEN align, const char* text, uint32_t charCount = 0xFFFFFFFF )			{
		return ::nwol::lineToRect(&display.Cells[0][0], _sizeWidth, _sizeHeight, offsetLine, offsetColumn, align, text, charCount);
	}

	template<size_t _sizeWidth, size_t _sizeHeight>
	int32_t											lineToGridColored				( ::nwol::SGrid<char, _sizeWidth, _sizeHeight>& display, SGrid<uint16_t, _sizeWidth, _sizeHeight>& textAttributes, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, ::nwol::ALIGN_SCREEN align, const char* text, uint32_t charCount = 0xFFFFFFFF ) {
		int32_t												actualX							= ::nwol::lineToRect(&display.Cells[0][0], _sizeWidth, _sizeHeight, offsetLine, offsetColumn, align, text, charCount);
		::nwol::valueToGrid(textAttributes, offsetLine, actualX, ::nwol::SCREEN_LEFT, &messageColor, 1, (int32_t)::std::min((uint32_t)::strlen(text), charCount));
		return actualX;
	}

	template<typename _tCell, size_t _sizeWidth, size_t _sizeHeight, typename... _Args>
	int32_t											printfToGrid					( ::nwol::SGrid<_tCell, _sizeWidth, _sizeHeight>& display, int32_t offsetLine, int32_t offsetColumn, ::nwol::ALIGN_SCREEN align, const char* format, _Args&&... args )						{
		int32_t												actualX							= ::nwol::printfToRect(&display.Cells[0][0], _sizeWidth, _sizeHeight, offsetLine, offsetColumn, align, format, args...);
		return actualX;
	}

	template<typename _tCell, size_t _sizeWidth, size_t _sizeHeight, typename... _Args>
	int32_t											printfToGridColored				( ::nwol::SGrid<_tCell, _sizeWidth, _sizeHeight>& display, SGrid<uint16_t, _sizeWidth, _sizeHeight>& textAttributes, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, ::nwol::ALIGN_SCREEN align, const char* format, _Args&&... args ) {
		char												precookStr[1024]				= {};
		int32_t												precookLen						= ::sprintf_s(precookStr, format, args...);
		int32_t												actualX							= ::lineToGridColored(display, textAttributes, messageColor, offsetLine, offsetColumn, align, precookStr);
		return actualX;
	}

#pragma pack(pop)

} // namespace

#endif // __TILE_H__973098721983761298736129873691872361__