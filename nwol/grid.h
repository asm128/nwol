/// Copyright 2016-2017 - asm128
#include "grid_view.h"

#include "nwol_text.h"
#include "nwol_noise.h"

#ifndef NWOL_GRID_H_973098721983761298736129873691872361
#define NWOL_GRID_H_973098721983761298736129873691872361

namespace nwol
{
#pragma pack(push, 1)

	template<typename _tCell, size_t _sizeWidth, size_t _sizeDepth>
	struct SGrid : public grid_view<_tCell> {
							typedef						_tCell									TCell;

		static constexpr	const uint32_t				Width									= (uint32_t)_sizeWidth;
		static constexpr	const uint32_t				Depth									= (uint32_t)_sizeDepth;

							TCell						Cells[_sizeDepth][_sizeWidth]			= {};

		inline constexpr								SGrid									()						: grid_view(&Cells[0][0], _sizeWidth, _sizeDepth)									{}
	};

	template<typename _tCell, size_t _sizeWidth, size_t _sizeDepth>
	struct grid_static : public grid_view<_tCell> {
							typedef						_tCell									TCell;
							typedef						grid_view<_tCell>						TGridView;

		static constexpr	const uint32_t				Width									= (uint32_t)_sizeWidth;
		static constexpr	const uint32_t				Depth									= (uint32_t)_sizeDepth;

							TCell						Cells[_sizeDepth][_sizeWidth]			= {};

		inline constexpr								grid_static								()						: grid_view(&Cells[0][0], _sizeWidth, _sizeDepth)									{}

		inline				::nwol::error_t				read															(const byte_t* input, uint32_t* inout_bytesRead)									{
			ree_if(0 == input, "Invalid input pointer!");
			TGridView											inputGrid														= {(_tCell*)input, Width, Depth};
			*inout_bytesRead								+= sizeof(_tCell) * size();
			for(uint32_t y = 0; y < Depth; ++y) {
				for(uint32_t x = 0; x < Width; ++x)
					::nwol::podcpy(&Cells[y][x], inputGrid[y][x]);
			}
			return 0;
		}

		inline				::nwol::error_t				write															(byte_t* input, uint32_t* inout_bytesWritten)					const				{
			ree_if(0 == input && 0 == inout_bytesWritten, "Invalid input!");
			if(0 != inout_bytesWritten)
				*inout_bytesWritten								+= sizeof(_tCell) * size();	// Just return the size required to store this.

			if(0 == input) 
				return 0;

			TGridView											newStorage														= {(_tCell*)input, Width, Depth};
			for(uint32_t y = 0; y < Depth; ++y) {
				for(uint32_t x = 0; x < Width; ++x)
					::nwol::podcpy(&newStorage[i], &Data[i]);
			}
			return 0;
		}
	};

	template<typename _tCell>
	void											fillCellsFromNoise				(::nwol::grid_view<_tCell>& grid, const _tCell& value, int64_t seed, const _tCell& clearValue = ' ', int32_t diceFaces=10)														{
		_tCell												* cells							= grid.begin();
		for(uint32_t i = 0, count = grid.size();  i < count; ++i) {
			double												noise							= ::nwol::noiseNormal1D(i + 1, seed);
			int32_t												dice							= int32_t(noise * diceFaces);
			if(0 == dice)
				cells[i]										= value;
		}
	}

	template<typename _tCell>
	void											clearGrid						(::nwol::grid_view<_tCell>& grid, const _tCell& clearValue = ' ')																												{
		_tCell												* cells							= grid.begin();
		for(uint32_t i = 0, count = grid.size(); i < count; ++i )
			cells[i]										= clearValue;
	}

	template<typename _tCell>
	void											drawGridBorders					( ::nwol::grid_view<_tCell>& grid, const _tCell& value)																												{
		for(uint32_t z = 0, maxZ = grid.height(); z < maxZ; ++z) {
			grid[z][0]										= value; 
			grid[z][grid.width() - 1]						= value; 
		}
		for(uint32_t x=0, maxX = grid.width() ; x < maxX; ++x) {
			grid[0][x]										= value; 
			grid[grid.height() - 1][x]						= value; 
		}
	}

	template<typename _tCell>
	inline			int32_t							valueToGrid						( ::nwol::grid_view<_tCell>& grid, int32_t offsetLine, int32_t offsetColumn, ::nwol::ALIGN_SCREEN align, const _tCell* values, int32_t valueCount, int32_t repeat = 0 )	{
		return ::nwol::valueToRect(grid.begin(), grid.width(), grid.height(), offsetLine, offsetColumn, align, values, valueCount, repeat);
	}

	static inline	int32_t							lineToGrid						( ::nwol::grid_view<char>& grid, int32_t offsetLine, int32_t offsetColumn, ::nwol::ALIGN_SCREEN align, const char* text, uint32_t charCount = 0xFFFFFFFF )				{
		return ::nwol::lineToRect(grid.begin(), grid.width(), grid.height(), offsetLine, offsetColumn, align, text, charCount);
	}

	static inline	int32_t							lineToGridColored				( ::nwol::grid_view<char>& display, ::nwol::grid_view<uint16_t>& textAttributes, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, ::nwol::ALIGN_SCREEN align, const char* text, uint32_t charCount = 0xFFFFFFFF ) {
		int32_t												actualX							= ::nwol::lineToRect(display.begin(), display.width(), display.height(), offsetLine, offsetColumn, align, text, charCount);
		::nwol::valueToGrid(textAttributes, offsetLine, actualX, ::nwol::SCREEN_LEFT, &messageColor, 1, (int32_t)::nwol::min((uint32_t)::strlen(text), charCount));
		return actualX;
	}

	template<typename _tCell, typename... _Args>
	int32_t											printfToGrid					( ::nwol::grid_view<_tCell>& display, int32_t offsetLine, int32_t offsetColumn, ::nwol::ALIGN_SCREEN align, const char* format, _Args&&... args )						{
		int32_t												actualX							= ::nwol::printfToRect(display.begin(), display.width(), display.height(), offsetLine, offsetColumn, align, format, args...);
		return actualX;
	}

	template<typename _tCell, typename... _Args>
	int32_t											printfToGridColored				( ::nwol::grid_view<_tCell>& display, ::nwol::grid_view<uint16_t>& textAttributes, uint16_t messageColor, int32_t offsetLine, int32_t offsetColumn, ::nwol::ALIGN_SCREEN align, const char* format, _Args&&... args ) {
		char												precookStr[1024]				= {};
		int32_t												precookLen						= ::sprintf_s(precookStr, format, args...);
		int32_t												actualX							= ::nwol::lineToGridColored(display, textAttributes, messageColor, offsetLine, offsetColumn, align, precookStr);
		return actualX;
	}

#pragma pack(pop)

} // namespace

#endif // NWOL_GRID_H_973098721983761298736129873691872361
