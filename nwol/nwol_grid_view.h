/// Copyright 2016-2017 - asm128
#include "nwol_array_view.h"

#ifndef NWOL_GRID_VIEW_H_926340923746098743230948
#define NWOL_GRID_VIEW_H_926340923746098743230948

namespace nwol
{
#pragma pack(push, 1)
	template<typename _tValue>
	class grid_view {
	protected:
		_tValue*												Data					= nullptr;
		uint32_t												Width					= 0;
		uint32_t												Height					= 0;
	public:

		static				const uint32_t						ELEMENT_SIZE			= (uint32_t)sizeof(_tValue);
		//
		inline constexpr										grid_view				()														noexcept	= default;
		inline constexpr										grid_view				(const grid_view& other)								noexcept	= default;
		inline constexpr										grid_view				(_tValue* Data, uint32_t width, uint32_t height)		noexcept	: Data(Data), Width	((uint32_t)width), Height((uint32_t)height)		{}
																																				
		template <size_t _width, size_t _height>																								
		inline constexpr										grid_view				(const	_tValue (&Data)[_height][_width])				noexcept	: Data(Data), Width	((uint32_t)_width), Height((uint32_t)_height)	{}
																																				
		template <size_t _width, size_t _height>																								
		inline constexpr										grid_view				(		_tValue	(&Data)[_height][_width])				noexcept	: Data(Data), Width	((uint32_t)_width), Height((uint32_t)_height)	{}

		// Report error before dereferencing invalid index.
		inline				grid_view<_tValue>&					operator=				(const grid_view& other)								noexcept	= default;
		inline				array_view<const _tValue>			operator[]				(uint32_t row)									const	noexcept	{ 
			retval_error_if	({}, 0 == Data		, "Uninitialized pointer."); 
			retval_error_if	({}, row >= Height	, "%s: %u.", "Invalid grid row"	, row); 
			return array_view<const _tValue>(&Data[row*Width], Width);
		}

		// Report error before dereferencing invalid index.
		inline				array_view<_tValue>					operator[]				(uint32_t row)											noexcept	{ 
			retval_error_if	({}, 0 == Data		, "Uninitialized pointer."); 
			retval_error_if	({}, row >= Height, "%s: %u.", "Invalid grid row", row); 
			return array_view<_tValue>(&Data[row*Width], Width);
		}

		inline constexpr	uint32_t							width					()												const	noexcept	{ return Width;					}
		inline constexpr	uint32_t							height					()												const	noexcept	{ return Height;				}

		inline				_tValue*							begin					()														noexcept	{ return Data;					}
		inline				 _tValue*							end						()														noexcept	{ return Data + size();			}
		inline constexpr	const _tValue*						begin					()												const	noexcept	{ return Data;					}
		inline constexpr	const _tValue*						end						()												const	noexcept	{ return Data + size();			}
		inline constexpr	uint32_t							size					()												const	noexcept	{ return Width * Height;		}
		//inline constexpr	bool								empty					()												const	noexcept	{ return 0 == (Width * Height);	}
	};

	// grid_view common typedefs
	typedef				::nwol::grid_view<char_t			>	grid_char			;
	typedef				::nwol::grid_view<ubyte_t			>	grid_ubyte			;
	typedef				::nwol::grid_view<byte_t			>	grid_byte			;
	typedef				::nwol::grid_view<float32_t			>	grid_float32		;
	typedef				::nwol::grid_view<float64_t			>	grid_float64		;
	typedef				::nwol::grid_view<uint8_t			>	grid_uint8			;
	typedef				::nwol::grid_view<uint16_t			>	grid_uint16			;
	typedef				::nwol::grid_view<uint32_t			>	grid_uint32			;
	typedef				::nwol::grid_view<uint64_t			>	grid_uint64			;
	typedef				::nwol::grid_view<int8_t			>	grid_int8			;
	typedef				::nwol::grid_view<int16_t			>	grid_int16			;
	typedef				::nwol::grid_view<int32_t			>	grid_int32			;
	typedef				::nwol::grid_view<int64_t			>	grid_int64			;

	// grid_view<const> common typedefs
	typedef				::nwol::grid_view<const char_t		>	grid_const_char		;
	typedef				::nwol::grid_view<const ubyte_t		>	grid_const_ubyte	;
	typedef				::nwol::grid_view<const byte_t		>	grid_const_byte		;
	typedef				::nwol::grid_view<const float32_t	>	grid_const_float32	;
	typedef				::nwol::grid_view<const float64_t	>	grid_const_float64	;
	typedef				::nwol::grid_view<const uint8_t		>	grid_const_uint8	;
	typedef				::nwol::grid_view<const uint16_t	>	grid_const_uint16	;
	typedef				::nwol::grid_view<const uint32_t	>	grid_const_uint32	;
	typedef				::nwol::grid_view<const uint64_t	>	grid_const_uint64	;
	typedef				::nwol::grid_view<const int8_t		>	grid_const_int8		;
	typedef				::nwol::grid_view<const int16_t		>	grid_const_int16	;
	typedef				::nwol::grid_view<const int32_t		>	grid_const_int32	;
	typedef				::nwol::grid_view<const int64_t		>	grid_const_int64	;
#pragma pack(pop)


}

#endif // NWOL_GRID_VIEW_H_926340923746098743230948

