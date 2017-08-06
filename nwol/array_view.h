/// Copyright 2016-2017 - asm128
#include "nwol_log.h"

#ifndef ARRAY_VIEW_H_926340923746098743230948
#define ARRAY_VIEW_H_926340923746098743230948

namespace nwol
{
#pragma pack(push, 1)
	template<typename _tElement>
	class array_view {
	protected:
							_tElement								* Data					= nullptr;
							uint32_t								Count					= 0;
	public:
		//typedef			array_view<_tElement>					TView					;
		typedef				_tElement								TElement				;
		static				const uint32_t							ELEMENT_SIZE			= (uint32_t)sizeof(_tElement);

		template<size_t _arraySize>
		inline constexpr											array_view				(_tElement (&data)[_arraySize])								noexcept	: Data(data), Count((uint32_t)_arraySize)		{}
		inline constexpr											array_view				()															noexcept	= default;
		inline														array_view				(_tElement* data, uint32_t count)										: Data(data), Count(data ? count : 0U)			{ 
			throw_if((nullptr == data) && count, "", "array_view initialized with a null pointer and a counter with value %u", count); 
		}
		//inline				const _tElement&						operator []				(uint32_t index)									const				{ throw_if(0 == Data, "", "%s", "Uninitialized array pointer!"); throw_if(index >= Count, "%s: %u.", "Invalid array index.", index); return Data[index]; }
		//inline				_tElement&								operator []				(uint32_t index)														{ throw_if(0 == Data, "", "%s", "Uninitialized array pointer!"); throw_if(index >= Count, "%s: %u.", "Invalid array index.", index); return Data[index]; }
							_tElement&								operator []				(uint32_t index)									const				{ throw_if(0 == Data, "", "%s", "Uninitialized array pointer!"); throw_if(index >= Count, "%s: %u.", "Invalid array index.", index); return Data[index]; }

		inline constexpr	operator								bool					()													const	noexcept	{ return Count > 0 ? true : false;				}

		inline constexpr	_tElement*								begin					()													const	noexcept	{ return Data;									}
		inline constexpr	_tElement*								end						()													const	noexcept	{ return Data + Count;							}
		inline constexpr	bool									empty					()													const	noexcept	{ return 0 == Count;							}
		inline constexpr	uint32_t								size					()													const	noexcept	{ return Count;									}

		inline constexpr	uint32_t								byte_count				()													const	noexcept	{ return Count * (uint32_t)sizeof(_tElement);	}
		inline constexpr	byte_t*									byte					()													const	noexcept	{ return (byte_t*)Data;							}
		inline constexpr	const byte_t*							byte_const				()													const	noexcept	{ return (const byte_t*)Data;					}

		//inline constexpr								byte_const				()													const	noexcept	{ return (const byte_t*)Data;					}
	};

	// array_view common typedefs
	typedef				::nwol::array_view<char_t			>	string_view			;
	typedef				::nwol::array_view<ubyte_t			>	ubyte_view			;
	typedef				::nwol::array_view<byte_t			>	byte_view			;
	typedef				::nwol::array_view<float32_t		>	float32_view		;
	typedef				::nwol::array_view<float64_t		>	float64_view		;
	typedef				::nwol::array_view<uint8_t			>	uint8_view			;
	typedef				::nwol::array_view<uint16_t			>	uint16_view			;
	typedef				::nwol::array_view<uint32_t			>	uint32_view			;
	typedef				::nwol::array_view<uint64_t			>	uint64_view			;
	typedef				::nwol::array_view<int8_t			>	int8_view			;
	typedef				::nwol::array_view<int16_t			>	int16_view			;
	typedef				::nwol::array_view<int32_t			>	int32_view			;
	typedef				::nwol::array_view<int64_t			>	int64_view			;

	// array_view<const> common typedefs
	typedef				::nwol::array_view<const char_t		>	const_string_view	;
	typedef				::nwol::array_view<const ubyte_t	>	const_ubyte_view	;
	typedef				::nwol::array_view<const byte_t		>	const_byte_view		;
	typedef				::nwol::array_view<const float32_t	>	const_float32_view	;
	typedef				::nwol::array_view<const float64_t	>	const_float64_view	;
	typedef				::nwol::array_view<const uint8_t	>	const_uint8_view	;
	typedef				::nwol::array_view<const uint16_t	>	const_uint16_view	;
	typedef				::nwol::array_view<const uint32_t	>	const_uint32_view	;
	typedef				::nwol::array_view<const uint64_t	>	const_uint64_view	;
	typedef				::nwol::array_view<const int8_t		>	const_int8_view		;
	typedef				::nwol::array_view<const int16_t	>	const_int16_view	;
	typedef				::nwol::array_view<const int32_t	>	const_int32_view	;
	typedef				::nwol::array_view<const int64_t	>	const_int64_view	;
#pragma pack(pop)
}

#endif // ARRAY_VIEW_H_926340923746098743230948
