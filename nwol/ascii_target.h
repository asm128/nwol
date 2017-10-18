/// Copyright 2017 - asm128
#include "nwol_grid_view.h"

#ifndef ASCII_TARGET_H_209834723428374234
#define ASCII_TARGET_H_209834723428374234

namespace nwol
{
	struct SASCIITarget_old {
							::nwol::grid_view<char		>					Text										= {};
							::nwol::grid_view<uint16_t	>					Attributes									= {};
							::nwol::array_view<uint32_t	>					Palette										= {};

		inline constexpr	uint32_t										Width										()						const	noexcept	{ return Text.width	(); }
		inline constexpr	uint32_t										Height										()						const	noexcept	{ return Text.height(); }
	};	// struct

}	// namespace

#endif	//	ASCII_TARGET_H_209834723428374234
