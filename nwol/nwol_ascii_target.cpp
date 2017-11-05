#include "nwol_ascii_target.h"
#include "nwol_memory.h"
#include "ascii_color.h"
#include "nwol_fill.h"

				::nwol::error_t									nwol::asciiTargetCreate							(::nwol::SASCIITarget& console, uint32_t width, uint32_t height)		{ 
	console.Characters												= {(uint8_t *)::nwol::nwol_malloc(sizeof(uint8_t ) * width * height), width, height}; 
	console.Colors													= {(uint16_t*)::nwol::nwol_malloc(sizeof(uint16_t) * width * height), width, height}; 
	return 0; 
}

				::nwol::error_t									nwol::asciiTargetDestroy						(::nwol::SASCIITarget& console)											{ 
	if(console.Characters	.begin()) ::nwol::nwol_free(console.Characters	.begin());
	if(console.Colors		.begin()) ::nwol::nwol_free(console.Colors		.begin());
	return 0; 
}

				::nwol::error_t									nwol::asciiTargetClear							(::nwol::SASCIITarget& console, uint8_t character, uint16_t color)		{ 
	::nwol::fillArray(console.Characters	.begin(), character	, console.Characters	.size(), console.Characters	.size());
	::nwol::fillArray(console.Colors		.begin(), color		, console.Colors		.size(), console.Colors		.size());
	return 0; 
}
