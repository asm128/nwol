/// Copyright 2017 - asm128
#include "nwol_grid_view.h"
#include "nwol_color.h"
#include "nwol_coord.h"
#include "ascii_color.h"

#ifndef NWOL_ASCII_TARGET_H_9823740982374
#define NWOL_ASCII_TARGET_H_9823740982374

namespace nwol 
{
#pragma pack(push, 1)
	struct SASCIITarget {
								::nwol::grid_view<uint8_t>						Characters									= {};
								::nwol::grid_view<uint16_t>						Colors										= {};

								::nwol::array_view<::nwol::SColorRGBA>			Palette										= {};

		inline constexpr		uint32_t										Width										()															const	noexcept	{ return Characters.width	(); }
		inline constexpr		uint32_t										Height										()															const	noexcept	{ return Characters.height	(); }
	};
#pragma pack(pop)
							::nwol::error_t									asciiTargetCreate							(::nwol::SASCIITarget& console, uint32_t width, uint32_t height);
							::nwol::error_t									asciiTargetDestroy							(::nwol::SASCIITarget& console);
							::nwol::error_t									asciiTargetClear							(::nwol::SASCIITarget& console, uint8_t character = ' ', uint16_t color = ::nwol::ASCII_COLOR_INDEX_WHITE);	

	template<typename _tUnit>
	static inline			::nwol::error_t									asciiTargetCreate							(::nwol::SASCIITarget& console, const SCoord2<_tUnit>& size)	{ return asciiTargetCreate(console, (uint32_t)size.x, (uint32_t)size.y); }
} // namespace 

#endif // NWOL_ASCII_TARGET_H_9823740982374
