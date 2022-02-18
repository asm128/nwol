/// Copyright 2015-2017 - asm128
#include "gui.h"
#include "nwol_fill.h"

::nwol::error_t						nwol::createControl			(::nwol::SGUI& guiSystem, const ::nwol::SGUIControl& definition)						{
	::nwol::SRectangle2D<int32_t>			areaRealigned				= {};
	::nwol::realignRectangle(guiSystem.TargetSizeASCII.x, guiSystem.TargetSizeASCII.y, definition.AreaASCII, areaRealigned, definition.AlignArea);

	::nwol::SGUIControlTable				& controls					= guiSystem.Controls;
	int32_t									oldSize						= controls.AreasRealignedASCII.size();
	int32_t									resultCheck	[]				= 
		{	controls.AreasRealignedASCII	.push_back(areaRealigned				)
		,	controls.AreasASCII				.push_back(definition.AreaASCII			)
		,	controls.AlignArea				.push_back(definition.AlignArea			)
		,	controls.AlignText				.push_back(definition.AlignText			)
		,	controls.TextColorsASCII		.push_back(definition.TextColorsASCII	)
		,	controls.TextColors32			.push_back(definition.TextColors32		)
		,	controls.ControlFlags			.push_back(definition.ControlFlags		)
		,	controls.Text					.push_back(definition.Text				)
		};

	for(uint32_t iCheck = 0, checkCount = ::nwol::size(resultCheck)-1; iCheck < checkCount; ++iCheck )
		if(resultCheck[iCheck] != resultCheck[iCheck+1]) {
			error_printf("Control property indices don't match. The reasons for this to happen are running out of memory or mismanagement of the control table.");
			guiSystem.Controls.Resize(oldSize);
			return -1;
		}
	return 0;
}

template<typename _tChar, typename _tCoord>
void								drawText					(_tChar* target, int32_t targetWidth, int32_t targetHeight, const char* text, int32_t textLen, const ::nwol::SRectangle2D<_tCoord>& rectangle, nwol::ALIGN_SCREEN alignText, bool bWordWrap)
{
	int32_t									ystart						= ::nwol::max(0, ::nwol::min(targetHeight	, rectangle.Offset.y					));
	int32_t									xstart						= ::nwol::max(0, ::nwol::min(targetWidth	, rectangle.Offset.x					));
	int32_t									ystop						= ::nwol::max(0, ::nwol::min(targetHeight	, rectangle.Offset.y+rectangle.Size.y	));
	int32_t									xstop						= ::nwol::max(0, ::nwol::min(targetWidth	, rectangle.Offset.x+rectangle.Size.x	));
	if(alignText == (::nwol::SCREEN_TOP_LEFT) && bWordWrap == false) {
		uint32_t								textCursor					= 0;
		uint32_t								trueTextLen					= (textLen == -1) ? 0xFFFFFFFF : (uint32_t)textLen;
		int32_t									targetY						= rectangle.Offset.y;
		int32_t									targetX						= rectangle.Offset.x; 
		while( textCursor < trueTextLen && text[textCursor] != 0 ) {
			if(text[textCursor] == '\r')  {
				int32_t									barraNIndex					= textCursor+1;
				if(barraNIndex < textLen && text[barraNIndex] == '\n') {
					++textCursor;
					++targetY;
					targetX								= rectangle.Offset.x;
				}
			}
			else if(text[textCursor] == '\n') {
				++targetY;
				targetX								= rectangle.Offset.x;
			}
			else {
				if(targetX >= xstart && targetX < xstop && targetY >= ystart && targetY < ystop) {
					int32_t							linearIndex		= targetY*targetWidth+targetX;
					target[linearIndex]					= text[textCursor];
				}
				++targetX;
			}

			if(targetY >= ystop)
				break;

			++textCursor;
		}
	}
	else if(alignText == (::nwol::SCREEN_TOP_RIGHT) && bWordWrap == true) {
		int32_t									targetY						= targetHeight	- rectangle.Offset.y;
		int32_t									targetX						= targetWidth	- rectangle.Offset.x; 
		uint32_t								textCursor					= 0;
		uint32_t								trueTextLen					= (textLen == -1) ? 0xFFFFFFFF : (uint32_t)textLen;
		while( textCursor < trueTextLen && text[textCursor] != 0 ) {		
			if(text[textCursor] == '\r') {
				int32_t									barraNIndex					= textCursor + 1;
				if(barraNIndex < textLen && text[barraNIndex] == '\n') {
					++textCursor;
					++targetY;
					targetX								= rectangle.Offset.x;
				}
			}
			else if(text[textCursor] == '\n') {
				++targetY;
				targetX								= rectangle.Offset.x;
			}
			else {
				if(targetX >= xstart && targetX < xstop && targetY >= ystart && targetY < ystop) {
					int32_t									linearIndex					= targetY*targetWidth+targetX;
					target[linearIndex]					= text[textCursor];
				}
				++targetX;
			}
			++textCursor;
		}
	}
}

::nwol::error_t						nwol::updateGUI				(::nwol::SGUI& GUISystem, const ::nwol::SInput& inputSystem)							{ 
	int32_t									mouseX						= GUISystem.MousePosition.x = inputSystem.Mouse.Deltas.x;
	int32_t									mouseY						= GUISystem.MousePosition.y = inputSystem.Mouse.Deltas.y;
	::nwol::SGUIControlTable				& controls					= GUISystem.Controls;
	for(uint32_t iControl=0; iControl < controls.AreasASCII.size(); ++iControl) {
		::nwol::SRectangle2D<int32_t>			& controlArea				= controls.AreasRealignedASCII[iControl];
		::nwol::CONTROL_STATE					& controlFlags				= controls.ControlFlags[iControl];
		
		// EXECUTE only lasts one tick.
		if( ::nwol::bit_true (controlFlags, ::nwol::CONTROL_STATE_EXECUTE))
			::nwol::bit_clear(controlFlags, ::nwol::CONTROL_STATE_EXECUTE);

		//--------------------
		if( ::nwol::in_range(mouseX, controlArea.Offset.x, controlArea.Offset.x + controlArea.Size.x) 
		 &&	::nwol::in_range(mouseY, controlArea.Offset.y, controlArea.Offset.y + controlArea.Size.y)
		) {
			if(::nwol::bit_true(controlFlags, ::nwol::CONTROL_STATE_MOUSE_OVER)) {
				if(inputSystem.ButtonDown(0) && ::nwol::bit_false(controlFlags, ::nwol::CONTROL_STATE_PRESSED)) 
					::nwol::bit_set		(controlFlags, ::nwol::CONTROL_STATE_PRESSED);
				else if(inputSystem.ButtonUp(0) && ::nwol::bit_true(controlFlags, ::nwol::CONTROL_STATE_PRESSED)) {
					::nwol::bit_set		(controlFlags, ::nwol::CONTROL_STATE_EXECUTE);
					::nwol::bit_clear	(controlFlags, ::nwol::CONTROL_STATE_PRESSED);
				}
			}
			else 
				::nwol::bit_set		(controlFlags, ::nwol::CONTROL_STATE_MOUSE_OVER);
		}
		else {
			if( ::nwol::bit_true (controlFlags, ::nwol::CONTROL_STATE_MOUSE_OVER))
				::nwol::bit_clear(controlFlags, ::nwol::CONTROL_STATE_MOUSE_OVER);

			if(inputSystem.ButtonUp(0) && ::nwol::bit_true(controlFlags, ::nwol::CONTROL_STATE_PRESSED))
				::nwol::bit_clear(controlFlags, ::nwol::CONTROL_STATE_PRESSED);
		}
	}
	return 0;
}

int32_t								nwol::renderGUIASCII		(char* bbText, uint16_t* bbColor	, const ::nwol::SGUI& guiSystem)					{
	const ::nwol::SCoord2<uint32_t>			& maxSize					= guiSystem.TargetSizeASCII;
	const ::nwol::SCoord2<int32_t>			& mousePos					= guiSystem.MousePosition;
	const ::nwol::SGUIControlTable			& controls					= guiSystem.Controls;

	// Draw rectangles
	for(uint32_t iRect=0; iRect < controls.AreasASCII.size(); ++iRect) {
		const ::nwol::SRectangle2D<int32_t>		& rectangle					= controls.AreasRealignedASCII	[iRect];
		::nwol::SControlTextColorASCII			textColor					= controls.TextColorsASCII		[iRect];
		if(::nwol::bit_true(controls.ControlFlags[iRect], ::nwol::CONTROL_STATE_MOUSE_OVER)) {
			uint8_t									color						= ::nwol::bit_true(controls.ControlFlags[iRect], ::nwol::CONTROL_STATE_PRESSED) ? textColor.ColorPressed.Background : textColor.Color.Background;
			textColor.Color.Background			= ::nwol::bit_true(controls.ControlFlags[iRect], ::nwol::CONTROL_STATE_PRESSED) ? textColor.ColorPressed.Foreground : textColor.Color.Foreground;
			textColor.Color.Foreground			= color;
		}
		::nwol::fillRectangle(bbColor, maxSize.x, maxSize.y, (uint16_t)textColor, rectangle);
	}

	// Draw text
	for(uint32_t iRect=0; iRect < controls.Text.size(); ++iRect) {
		const ::nwol::glabel					& label						= controls.Text					[iRect];
		const ::nwol::SRectangle2D<int32_t>		& rectangle					= controls.AreasRealignedASCII	[iRect];
		//const ::nwol::ALIGN_SCREEN				alignControl				= controls.AlignArea			[iRect];
		const ::nwol::ALIGN_SCREEN				alignText					= controls.AlignText			[iRect];
		::drawText(bbText, maxSize.x, maxSize.y, label.c_str(), label.size(), rectangle, alignText, false);
	}

	// Draw mouse cursor
	if( ::nwol::in_range((int32_t)mousePos.y, 0, (int32_t)maxSize.y) 
	 && ::nwol::in_range((int32_t)mousePos.x, 0, (int32_t)maxSize.x) 
	 ) {
		int32_t									linearIndex					= mousePos.y * maxSize.x + mousePos.x;
		bbColor[linearIndex]				= invert_color_ascii(bbColor[linearIndex]);
	}
	return 0;
}

::nwol::error_t						nwol::renderGUIBitmap		(uint32_t* bitmap, uint32_t width, uint32_t height, const ::nwol::SGUI& guiSystem)		{
	ree_if(0 == bitmap					, "Invalid target memory for rendering gui (nullptr).");
	ree_if(0 == width || 0 == height	, "Invalid target size: (width: %u, height: %u).", width, height);
	for(uint32_t iControl = 0, controlCount = guiSystem.Controls.ControlFlags.size(); iControl < controlCount; ++iControl) {
			
	}
	return 0;
}
