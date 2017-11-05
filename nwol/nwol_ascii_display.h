#include "nwol_ascii_target.h"

#ifndef NWOL_ASCII_CONSOLE_H_92384709283749082
#define NWOL_ASCII_CONSOLE_H_92384709283749082

namespace nwol
{
	struct SASCIIDisplayInfo {
						uint32_t										PaletteSize			;			// The amount of colors that can be stored in the palette. At the time of writing this comment, a maximum of 64k colors is supported.
						uint32_t										ColorWidth			;			// The color width in bytes.
						::nwol::SCoord2<uint32_t>						DisplaySizeMax		;			// In character units.
						::nwol::SCoord2<uint32_t>						DisplaySizeCurrent	;			// In character units.
						bool											Resize				;		
						bool											PaletteSet			;		
						bool											PaletteReset		;		
						bool											TitleSet			;
	};
//
//					::nwol::error_t									asciiDisplayPresentBGRA				(const ::nwol::array_view<const uint8_t>& characters, const ::nwol::array_view<const uint32_t>& colors);
					::nwol::error_t									asciiDisplayInfo					(SASCIIDisplayInfo& displayInfo);
					::nwol::error_t									asciiDisplayCreate					(uint32_t frontBufferWidth, uint32_t frontBufferHeight);
					::nwol::error_t									asciiDisplayDestroy					();
					::nwol::error_t									asciiDisplayPresent					(const ::nwol::array_view<const uint8_t>& characters, const ::nwol::array_view<const uint16_t>& colorRefs);
					::nwol::error_t									asciiDisplayClear					(uint8_t character, uint16_t colorRef);
					::nwol::error_t									asciiDisplaySize					(SCoord2<uint32_t>& size);

					::nwol::error_t									asciiDisplayResize					(uint32_t frontBufferWidth, uint32_t frontBufferHeight);
					::nwol::error_t									asciiDisplayPaletteSet				(const ::nwol::array_view<::nwol::SColorRGBA>& palette);
					::nwol::error_t									asciiDisplayPaletteReset			();
					::nwol::error_t									asciiDisplayTitleSet				(const ::nwol::array_view<const char>& title);

	template <typename _tUnit>
	static inline	::nwol::error_t									asciiDisplayCreate					(const ::nwol::SCoord2<_tUnit>& displaySize)				{ return ::nwol::asciiDisplayCreate(displaySize.x, displaySize.y); }
	static inline	::nwol::error_t									asciiDisplayPresent					(const ::nwol::SASCIITarget& renderTarget)					{
		return ::nwol::asciiDisplayPresent
			( array_view<const uint8_t >{renderTarget.Characters	.begin(), renderTarget.Characters	.size()}
			, array_view<const uint16_t>{renderTarget.Colors		.begin(), renderTarget.Colors		.size()}
			);
	}
}

#endif // NWOL_ASCII_CONSOLE_H_92384709283749082
