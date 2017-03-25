#include "nwol_enum.h"
#include "nwol_input.h"
#include "nwol_coord.h"
#include "ascii_color.h"
#include "ascii_target.h"

#ifndef __GUI_H__92834992384223849732__
#define __GUI_H__92834992384223849732__

namespace nwol
{
#pragma pack (push, 1)
	GDEFINE_ENUM_TYPE(CONTROL_FLAG, uint16_t);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, 0x000, NONE				);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, 0x001, WORD_WRAP			);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, 0x002, COLOR_INVERT		);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, 0x004, CLICKABLE			);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, 0x008, DISABLED			);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, 0x010, MOUSE_OVER			);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, 0x020, HIDDEN				);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, 0x040, ON_MOUSEOVER_INVERT	);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, 0x080, OCCLUDED			);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, 0x100, EXECUTE				);
	GDEFINE_ENUM_VALUE(CONTROL_FLAG, 0x200, PRESSED				);

	//--------------------------------------------------------- PODs
	struct SControlTextColorASCII	{
					STextColorASCII											Color				;
					STextColorASCII											ColorPressed		;

		inline		operator												uint16_t			()	const	{ return Color;										}	// This operator clamps the structure on purpose.
	};

	struct STextColor32				{
					uint32_t												Foreground			;
					uint32_t												Background			;
	};

	//--------------------------------------------------------- Objects
	struct SGUIControl				{				
					::nwol::SRectangle2D<int32_t>							AreaASCII			= {{1, 1}, {11, 1}}												;
					STextColor32											TextColors32		= {0xFFFFFFFF, 0xFF000000}										;
					SControlTextColorASCII									TextColorsASCII		= {{COLOR_BLUE, COLOR_DARKGREY}, {COLOR_YELLOW, COLOR_BLUE}}	;
					::nwol::ALIGN_SCREEN									AlignArea			= ::nwol::SCREEN_BOTTOM_RIGHT									;
					::nwol::ALIGN_SCREEN									AlignText			= ::nwol::SCREEN_TOP_LEFT										;
					CONTROL_FLAG											ControlFlags		= ::nwol::CONTROL_FLAG_NONE										;
					::nwol::glabel											Text				= {"GUI Control", (uint32_t)-1}									;
	};																				   

	struct SGUIControlTable			{				
					::nwol::array_pod	<::nwol::SRectangle2D<int32_t>	>	AreasRealigned		;
					::nwol::array_pod	<::nwol::SRectangle2D<int32_t>	>	AreasASCII			;
					::nwol::array_pod	<::nwol::ALIGN_SCREEN			>	AlignArea			;
					::nwol::array_pod	<::nwol::ALIGN_SCREEN			>	AlignText			;
					::nwol::array_pod	<SControlTextColorASCII			>	TextColorsASCII		;
					::nwol::array_pod	<STextColor32					>	TextColors32		;
					::nwol::array_pod	<CONTROL_FLAG					>	ControlFlags		;
					::nwol::array_obj	<::nwol::glabel					>	Text				;

					void													Clear				()						{
						::nwol::clear
							(	AreasRealigned		
							,	AreasASCII				
							,	AlignArea			
							,	AlignText			
							,	TextColorsASCII	
							,	TextColors32		
							,	ControlFlags		
							,	Text				
							);
					}
					void													Resize				(uint32_t newSize)		{
						::nwol::resize
							(	newSize
							,	AreasRealigned		
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
					::nwol::SCoord2<uint32_t>								TargetSize32		;
					::nwol::SCoord2<uint32_t>								TargetSizeASCII		;
					::nwol::SCoord2<int32_t>								MousePosition		;
					SGUIControlTable										Controls			;
	};
#pragma pack(pop)

	::nwol::error_t											createControl		(::nwol::SGUI& guiSystem, const ::nwol::SGUIControl	& definition	);
	::nwol::error_t											updateGUI			(::nwol::SGUI& guiSystem, const ::nwol::SInput		& inputSystem	);
	::nwol::error_t											renderGUIASCII		(char* bbText, uint16_t* bbColor, const ::nwol::SGUI& guiSystem		);
	::nwol::error_t											renderGUIASCII		(::nwol::SASCIITarget& target	, const ::nwol::SGUI& guiSystem		);
	

}//namespace

#endif // __GUI_H__92834992384223849732__