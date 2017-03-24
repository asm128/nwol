#include "grid_view.h"

#ifndef __ASCII_TARGET_H__209834723428374234__
#define __ASCII_TARGET_H__209834723428374234__

namespace nwol
{
	struct SASCIITarget
	{
							::nwol::grid_view<char		>	Text		= {};
							::nwol::grid_view<uint16_t	>	Attributes	= {};

		inline constexpr	uint32_t						Width		()		const	noexcept	{ return Text.width	(); }
		inline constexpr	uint32_t						Height		()		const	noexcept	{ return Text.height(); }
	};	// struct
}	// namespace

#endif	//	__ASCII_TARGET_H__209834723428374234__