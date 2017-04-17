#include "nwol_coord.h"

#ifndef __MISC_H__982143092634987160987328097642398__
#define __MISC_H__982143092634987160987328097642398__

namespace nwol
{
#pragma pack(push, 1)

	// This function copies an amount of _tCell values into a rectangle of _tCells 
	// with rectangleTopLeft specifying the origin of the rectangle 
	// and width and height specifying the sizes from there (with Y axis being up to down most likely)
	template<typename _tCell>	int32_t		valueToRect				( _tCell* rectangleTopLeft, uint32_t width, uint32_t height, int32_t offsetLine, int32_t offsetColumn, ALIGN_SCREEN align, const _tCell* values, int32_t valueCount, int32_t repeat=0 )	{
		if(align & SCREEN_RIGHT) {	// testing for RIGHT bit would tell us that this might be aligned to either RIGHT or CENTER.
			int32_t										offsetX					= (int32_t) ((align == SCREEN_CENTER) ? (width>>1)-(valueCount>>1)+offsetColumn : (int32_t)(width)-(valueCount)-offsetColumn); 
			return valueToRect(rectangleTopLeft, width, height, offsetLine, offsetX, SCREEN_LEFT, values, valueCount, repeat);
		}
		else if(offsetLine < (int32_t)height) {
			int32_t										linearCellOffset		= (int32_t)(offsetLine*width+offsetColumn);
			int32_t										remainingColumns		= (int32_t)width-offsetColumn;
			if(remainingColumns <= 0)
				return offsetColumn;
			int32_t										maxPrintSize			= (valueCount < remainingColumns) ? valueCount : remainingColumns;
			memcpy(&rectangleTopLeft[linearCellOffset], values, maxPrintSize*sizeof(_tCell));
			if(repeat > 0)
				valueToRect( rectangleTopLeft, width, height, offsetLine, offsetColumn+valueCount, align, values, valueCount, --repeat );
		}

		return offsetColumn;
	}

#pragma pack(pop)
}

#endif // __MISC_H__982143092634987160987328097642398__