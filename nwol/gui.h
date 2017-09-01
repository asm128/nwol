/// Copyright 2015-2017 - asm128
#include "nwol_enum.h"
#include "nwol_input.h"
#include "nwol_coord.h"
#include "ascii_color.h"
#include "ascii_target.h"

#ifndef NWOL_GUI_H_92834992384223849732
#define NWOL_GUI_H_92834992384223849732

namespace nwol
{
#pragma pack (push, 1)
	GDEFINE_ENUM_TYPE(CONTROL_FLAG, uint16_t);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, NONE				, 0x000	);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, WORD_WRAP			, 0x001	);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, COLOR_INVERT		, 0x002	);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, CLICKABLE			, 0x004	);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, DISABLED			, 0x008	);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, MOUSE_OVER			, 0x010	);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, HIDDEN				, 0x020	);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, ON_MOUSEOVER_INVERT, 0x040	);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, OCCLUDED			, 0x080	);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, EXECUTE			, 0x100	);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, PRESSED			, 0x200	);

	//--------------------------------------------------------- PODs
	struct SControlTextColorASCII	{
					::nwol::STextColorASCII										Color							;
					::nwol::STextColorASCII										ColorPressed					;

		inline		operator													uint16_t						()	const	noexcept	{ return Color; }	// This operator clamps the structure on purpose.
	};

	struct STextColor32				{
					uint32_t													Foreground						;
					uint32_t													Background						;
	};

	//--------------------------------------------------------- Objects
	struct SGUIControl				{				
					::nwol::glabel												Text							; //= {"GUI Control", 0xFF}											;
					::nwol::SRectangle2D<int32_t>								AreaASCII						; //= {{1, 1}, {11, 1}}												;
					::nwol::SRectangle2D<int32_t>								AreaBitmap						; //= {{1, 1}, {11, 1}}												;
					::nwol::STextColor32										TextColors32					; //= {0xFFFFFFFFL, 0xFF000000L}									;
					::nwol::SControlTextColorASCII								TextColorsASCII					; //= {{COLOR_BLUE, COLOR_DARKGREY}, {COLOR_YELLOW, COLOR_BLUE}}	;
					::nwol::ALIGN_SCREEN										AlignArea						; //= ::nwol::SCREEN_BOTTOM_RIGHT									;
					::nwol::ALIGN_SCREEN										AlignText						; //= ::nwol::SCREEN_TOP_LEFT										;
					::nwol::CONTROL_FLAG										ControlFlags					; //= ::nwol::CONTROL_FLAG_NONE										;

																				SGUIControl			
		( const ::nwol::glabel					& text				= {"GUI Control", 0xFF}
		, const ::nwol::SRectangle2D<int32_t>	& areaASCII			= {{1, 1}, {11, 1}}	
		, const ::nwol::SRectangle2D<int32_t>	& areaBitmap		= {{1, 1}, {11, 1}}	
		, const ::nwol::STextColor32			& textColors32		= {0xFFFFFFFFL, 0xFF000000L}
		, const ::nwol::SControlTextColorASCII	& textColorsASCII	= {{COLOR_BLUE, COLOR_DARKGREY}, {COLOR_YELLOW, COLOR_BLUE}}	
		, const ::nwol::ALIGN_SCREEN			& alignArea			= ::nwol::SCREEN_BOTTOM_RIGHT
		, const ::nwol::ALIGN_SCREEN			& alignText			= ::nwol::SCREEN_TOP_LEFT	
		, const ::nwol::CONTROL_FLAG			& controlFlags		= ::nwol::CONTROL_FLAG_NONE	
		) 
			:	Text			(text				)
			,	AreaASCII		(areaASCII			)
			,	AreaBitmap		(areaBitmap			)
			,	TextColors32	(textColors32		)
			,	TextColorsASCII	(textColorsASCII	)	
			,	AlignArea		(alignArea			)
			,	AlignText		(alignText			)
			,	ControlFlags	(controlFlags		)
		{}
	};

	struct SGUIControlTable	{				
					::nwol::array_pod<::nwol::SRectangle2D<int32_t>		>		AreasRealignedASCII				= {};
					::nwol::array_pod<::nwol::SRectangle2D<int32_t>		>		AreasRealignedBitmap			= {};
					::nwol::array_pod<::nwol::SRectangle2D<int32_t>		>		AreasASCII						= {};
					::nwol::array_pod<::nwol::SRectangle2D<int32_t>		>		AreasBitmap						= {};
					::nwol::array_pod<::nwol::ALIGN_SCREEN				>		AlignArea						= {};
					::nwol::array_pod<::nwol::ALIGN_SCREEN				>		AlignText						= {};
					::nwol::array_pod<::nwol::SControlTextColorASCII	>		TextColorsASCII					= {};
					::nwol::array_pod<::nwol::STextColor32				>		TextColors32					= {};
					::nwol::array_pod<::nwol::CONTROL_FLAG				>		ControlFlags					= {};
					::nwol::array_obj<::nwol::glabel					>		Text							= {};
					// -------------
					void														Clear							()						{
						::nwol::clear
							(	AreasRealignedASCII		
							,	AreasASCII				
							,	AlignArea			
							,	AlignText			
							,	TextColorsASCII	
							,	TextColors32		
							,	ControlFlags		
							,	Text				
							);
					}

					void														Resize							(uint32_t newSize)		{
						::nwol::resize
							(	newSize
							,	AreasRealignedASCII		
							,	AreasASCII				
							,	AlignArea			
							,	AlignText			
							,	TextColorsASCII	
							,	TextColors32		
							,	ControlFlags		
							,	Text				
							);
					}
	};

	struct SGUI {				
					::nwol::SCoord2<uint32_t>									TargetSizeBitmap				;
					::nwol::SCoord2<uint32_t>									TargetSizeASCII					;
					::nwol::SCoord2<int32_t>									MousePosition					;
					::nwol::SGUIControlTable									Controls						;
	};
#pragma pack(pop)
				// Controls
				::nwol::error_t												getControl						(::nwol::SGUI& guiSystem, ::nwol::SGUIControl		& definition	);
				::nwol::error_t												createControl					(::nwol::SGUI& guiSystem, const ::nwol::SGUIControl	& definition	);

				// Update/Render
				::nwol::error_t												updateGUI						(::nwol::SGUI& guiSystem, const ::nwol::SInput		& inputSystem	);
				::nwol::error_t												renderGUIASCII					(char* bbText, uint16_t* bbColor, const ::nwol::SGUI& guiSystem		);
				::nwol::error_t												renderGUIASCII					(::nwol::SASCIITarget& target	, const ::nwol::SGUI& guiSystem		);
				::nwol::error_t												renderGUIBitmap					(uint32_t* bitmap, uint32_t width, uint32_t height, const ::nwol::SGUI& guiSystem);
}//namespace

#endif // NWOL_GUI_H_92834992384223849732