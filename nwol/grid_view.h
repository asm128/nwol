/// Copyright 2016-2017 - asm128
#include "array_view.h"

#ifndef __GRID_VIEW_H__926340923746098743230948__
#define __GRID_VIEW_H__926340923746098743230948__

namespace nwol
{
#pragma pack(push, 1)
	template<typename _tValue>
	class grid_view {
	protected:
		_tValue*										Data			= nullptr;
		uint32_t										Width			= 0;
		uint32_t										Height			= 0;
	public:

		static				const uint32_t				element_size	= (uint32_t)sizeof(_tValue);
		//
		inline constexpr								grid_view		()														noexcept	= default;
		inline constexpr								grid_view		(const grid_view& other)								noexcept	= default;
		inline constexpr								grid_view		(_tValue* Data, uint32_t width, uint32_t height)		noexcept	: Data(Data), Width	((uint32_t)width), Height((uint32_t)height)		{}
																																
		template <size_t _width, size_t _height>																				
		inline constexpr								grid_view		(const	_tValue (&Data)[_height][_width])				noexcept	: Data(Data), Width	((uint32_t)_width), Height((uint32_t)_height)	{}
																																
		template <size_t _width, size_t _height>																				
		inline constexpr								grid_view		(		_tValue	(&Data)[_height][_width])				noexcept	: Data(Data), Width	((uint32_t)_width), Height((uint32_t)_height)	{}

		// Report error before dereferencing invalid index.
		inline				grid_view<_tValue>&			operator=		(const grid_view& other)								noexcept	= default;
		inline				array_view<const _tValue>	operator[]		(uint32_t row)									const	noexcept	{ 
			retval_error_if({}, row >= Height, "%s: %u", "Invalid grid row", row); 
			return array_view<const _tValue>(&Data[row*Width], Width);
		}

		// Report error before dereferencing invalid index.
		inline				array_view<_tValue>			operator[]		(uint32_t row)											noexcept	{ 
			retval_error_if({}, row >= Height, "%s: %u", "Invalid grid row", row); 
			return array_view<_tValue>(&Data[row*Width], Width);
		}

		inline constexpr	uint32_t					width			()												const	noexcept	{ return Width;					}
		inline constexpr	uint32_t					height			()												const	noexcept	{ return Height;				}

		//inline constexpr	bool						empty			()												const	noexcept	{ return 0 == Count;			}
		inline constexpr	uint32_t					size			()												const	noexcept	{ return Width * Height;		}
		inline				_tValue*					begin			()														noexcept	{ return Data;					}
		inline constexpr	const _tValue*				begin			()												const	noexcept	{ return Data;					}
		inline				 _tValue*					end				()														noexcept	{ return Data + size();			}
		inline constexpr	const _tValue*				end				()												const	noexcept	{ return Data + size();			}
	};
#pragma pack(pop)
}

#endif // __GRID_VIEW_H__926340923746098743230948__

