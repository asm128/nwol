#include "nwoc.h"

#include "text.h"
#include "ascii_screen.h"
#include "gui.h"

#include "runtime_impl.h"

#include <thread>
#include <process.h>

namespace nwol 
{
	struct SGUIControlBase			{				
					::nwol::glabel												Text					= {"GUI Control", 1024}											;
					::nwol::CONTROL_FLAG										ControlFlags			= ::nwol::CONTROL_FLAG_NONE										;
	
																				SGUIControlBase			
				(	const ::nwol::glabel					& text					= {"GUI Control", 1024}
				,	const ::nwol::CONTROL_FLAG				& controlFlags			= ::nwol::CONTROL_FLAG_NONE	
				) 
			:	Text			(text				)
			,	ControlFlags	(controlFlags		)
		{}
	};
	
	//--------------------------------------------------------- Objects
	struct SGUIControlBitmap				{	
					::nwol::SGUIControlBase										BaseProperties			= {}															;
					::nwol::SRectangle2D<int32_t>								AreaBitmap				= {{1, 1}, {11, 1}}												;
					::nwol::STextColor32										TextColors32			= {0xFFFFFFFFL, 0xFF000000L}									;
					::nwol::ALIGN_SCREEN										AlignArea				= ::nwol::SCREEN_BOTTOM_RIGHT									;
					::nwol::ALIGN_SCREEN										AlignText				= ::nwol::SCREEN_TOP_LEFT										;
	
																				SGUIControlBitmap			
				(	const ::nwol::SGUIControlBase			& baseProperties		= {}
				,	const ::nwol::SRectangle2D<int32_t>		& areaBitmap			= {{1, 1}, {11, 1}}	
				,	const ::nwol::STextColor32				& textColors32			= {0xFFFFFFFFL, 0xFF000000L}
				,	const ::nwol::ALIGN_SCREEN				& alignArea				= ::nwol::SCREEN_BOTTOM_RIGHT
				,	const ::nwol::ALIGN_SCREEN				& alignText				= ::nwol::SCREEN_TOP_LEFT	
				) 
			:	BaseProperties	(baseProperties		)
			,	AreaBitmap		(areaBitmap			)
			,	TextColors32	(textColors32		)
			,	AlignArea		(alignArea			)
			,	AlignText		(alignText			)
		{}
	};
	
	struct SGUIControlASCII				{				
					::nwol::SGUIControlBase										BaseProperties			= {}															;
					::nwol::SRectangle2D<int32_t>								AreaASCII				= {{1, 1}, {11, 1}}												;
					::nwol::SControlTextColorASCII								TextColorsASCII			= {{COLOR_BLUE, COLOR_DARKGREY}, {COLOR_YELLOW, COLOR_BLUE}}	;
					::nwol::ALIGN_SCREEN										AlignArea				= ::nwol::SCREEN_BOTTOM_RIGHT									;
					::nwol::ALIGN_SCREEN										AlignText				= ::nwol::SCREEN_TOP_LEFT										;
	
																				SGUIControlASCII			
				(	const ::nwol::SGUIControlBase			& baseProperties		= {}
				,	const ::nwol::SRectangle2D<int32_t>		& areaASCII				= {{1, 1}, {11, 1}}	
				,	const ::nwol::SControlTextColorASCII	& textColorsASCII		= {{COLOR_BLUE, COLOR_DARKGREY}, {COLOR_YELLOW, COLOR_BLUE}}	
				,	const ::nwol::ALIGN_SCREEN				& alignArea				= ::nwol::SCREEN_BOTTOM_RIGHT
				,	const ::nwol::ALIGN_SCREEN				& alignText				= ::nwol::SCREEN_TOP_LEFT	
				) 
			:	BaseProperties	(baseProperties		)
			,	AreaASCII		(areaASCII			)
			,	TextColorsASCII	(textColorsASCII	)	
			,	AlignArea		(alignArea			)
			,	AlignText		(alignText			)
		{}
	};
	
	//--------------------------------------------------------- Objects
	struct SGUIControlEx {				
					::nwol::SGUIControlBase										BaseProperties			;
					::nwol::SGUIControlASCII									ASCIIProperties			;
					::nwol::SGUIControlBitmap									BitmapProperties		;
	
																				SGUIControlEx
				(	const ::nwol::glabel					& text					= {"GUI Control", 1024}
				,	const ::nwol::SRectangle2D<int32_t>		& areaASCII				= {{ 1,  1}, { 11,  1}}	
				,	const ::nwol::SRectangle2D<int32_t>		& areaBitmap			= {{10, 10}, {110, 20}}	
				,	const ::nwol::STextColor32				& textColors32			= {0xFFFFFFFFL, 0xFF000000L}
				,	const ::nwol::SControlTextColorASCII	& textColorsASCII		= {{COLOR_BLUE, COLOR_DARKGREY}, {COLOR_YELLOW, COLOR_BLUE}}	
				,	const ::nwol::ALIGN_SCREEN				& alignArea				= ::nwol::SCREEN_BOTTOM_RIGHT
				,	const ::nwol::ALIGN_SCREEN				& alignText				= ::nwol::SCREEN_TOP_LEFT	
				,	const ::nwol::CONTROL_FLAG				& controlFlags			= ::nwol::CONTROL_FLAG_NONE	
				) 
			:	BaseProperties		(text, controlFlags	)
			,	ASCIIProperties		{{text, controlFlags}, areaASCII	, textColorsASCII	, alignArea, alignText}
			,	BitmapProperties	{{text, controlFlags}, areaBitmap	, textColors32		, alignArea, alignText}
		{}
	};
} // namespace

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(::SApplication, "No Workflow Overhead Application", 0, 1);

int32_t														cleanup									(::SApplication& instanceApp)							{ 
	::nwol::shutdownASCIIScreen();

	nwol_necall(::networkDisable(instanceApp), "Error when disabling network.");
	::std::this_thread::sleep_for(::std::chrono::milliseconds(1000));
	return 0; 
}

template<typename _tBase>
void														printTypeDebug							()														{
	static const _tBase::TRegistry									memberRegistry							= _tBase::get_member_registry();
	for(uint32_t iMember=0; iMember < memberRegistry.get_member_count(); ++iMember) {
		const ::nwol::STypeIdentifier									& identifier							= memberRegistry.get_types()[iMember];
		always_printf
			(	"0x%X: %s::%s %s (%s): %s."
			,	memberRegistry.get_data_type_ids	()[iMember]
			,	identifier.NameSpace	.c_str()
			,	identifier.Name			.c_str()
			,	memberRegistry.get_names			()[iMember].c_str()
			,	memberRegistry.get_display_names	()[iMember].c_str()
			,	memberRegistry.get_descriptions		()[iMember].c_str()
			);	
	}
}

#define DEFAULT_TEXT_COLOR_BUTTON_ASCII	::nwol::SControlTextColorASCII{{COLOR_BLUE, COLOR_DARKGREY}, {COLOR_YELLOW, COLOR_BLUE}}
#define DEFAULT_TEXT_COLOR_BUTTON_32	::nwol::STextColor32{0xFFFFFFFFL, 0xFF000000L}

const ::nwol::SGUIControlEx									controlDefinitions[]					= 
	{	::nwol::SGUIControlEx({"Exit"			, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control  1"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control  2"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control  3"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control  4"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control  5"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control  6"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control  7"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control  8"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control  9"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 10"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 11"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 12"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 13"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 14"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 15"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 16"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 17"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 18"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 19"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 20"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 21"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 22"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 23"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 24"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 25"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 26"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 27"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 28"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	,	::nwol::SGUIControlEx({"GUI Control 29"	, ~0U}, {{ 1,  1}, { 11,  1}}, {{10, 10}, {110, 20}}, DEFAULT_TEXT_COLOR_BUTTON_32, DEFAULT_TEXT_COLOR_BUTTON_ASCII, ::nwol::SCREEN_BOTTOM_RIGHT, ::nwol::SCREEN_TOP_LEFT)
	};

int32_t														setupGUI								(::SApplication& instanceApp)							{ 
	::nwol::SGUI													& guiSystem								= instanceApp.GUI;
	::nwol::array_static<::nwol::SGUIControlEx, ::nwol::size(controlDefinitions)>	
																	testStatic								= controlDefinitions;
	guiSystem.TargetSizeASCII.x									= ::nwol::getASCIIBackBufferWidth	();
	guiSystem.TargetSizeASCII.y									= ::nwol::getASCIIBackBufferHeight	();

	static const ::nwol::glabel										exitControlLabel							= "Exit";
	::nwol::SGUIControl												newControl;

	newControl.AreaASCII										= {1, 1, (int32_t)exitControlLabel.size(), 1}	;
	newControl.Text												= exitControlLabel								;
	nwol_necall(::nwol::createControl(guiSystem, newControl), "Failed to create control!");

	return 0;
}

::SApplication												* g_App									= nullptr;	// We need this for the WndProc()

int32_t														setup									(::SApplication& instanceApp)							{ 
	g_App														= &instanceApp;	// We need this for the WndProc()
	::nwol::SCoord2<uint32_t>										& sizeTargetASCII						= instanceApp.GUI.TargetSizeASCII;
	::nwol::initASCIIScreen(sizeTargetASCII.x, sizeTargetASCII.y);
	char															moduleTitle[240]						= {};
	uint8_t															moduleTitleLen							= (uint8_t)::nwol::size(moduleTitle);
	nwol_necall(::nwol_moduleTitle(moduleTitle, &moduleTitleLen), "If this fails then something weird is going on.");
	::nwol::setASCIIScreenTitle(moduleTitle);

	printTypeDebug<::SApplication>();
	printTypeDebug<::nwol::SApplicationNetworkClient>();

	nwol_necall(::networkEnable	(instanceApp), "Failed to enable network."	);
	nwol_necall(::setupGUI		(instanceApp), "Failed to setup GUI."		);

	return 0; 
}

#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif


int32_t														update									(::SApplication& instanceApp, bool exitRequested)		{
	if(exitRequested)
		return ::nwol::APPLICATION_STATE_EXIT;

	::nwol::SInput													& inputSystem							= instanceApp.Input;
	::nwol::pollInput(inputSystem);

	const ::nwol::SInput											& inputSystemConst						= instanceApp.Input;
	::nwol::SGUI													& guiSystem								= instanceApp.GUI;
	::nwol::updateGUI(guiSystem, inputSystemConst);

	::nwol::array_pod<::nwol::CONTROL_FLAG>							& controlFlags							= guiSystem.Controls.ControlFlags;

	for(uint32_t iControl = 0, controlCount = controlFlags.size(); iControl < controlCount; ++iControl)
		if(::nwol::bit_true(controlFlags[iControl], ::nwol::CONTROL_FLAG_EXECUTE)) {
			info_printf("Execute control %u.", iControl);
			switch(iControl) {
			case 0	:	return ::nwol::APPLICATION_STATE_EXIT;					break;
			default	:	warning_printf("Unhandled control id: %u.", iControl);	break;
			}
		}
	return 0; 
}

int32_t														render									(::SApplication& instanceApp)							{ 
	::nwol::clearASCIIBackBuffer(' ', COLOR_WHITE);
	::nwol::SASCIITarget											target;
	if(not_errored(::nwol::getASCIIBackBuffer(target))) {
		error_if(errored(::nwol::renderGUIASCII(target, instanceApp.GUI)), "%s", "Failed to render ASCII GUI!")
		else
			::nwol::presentASCIIBackBuffer();
	}
	else 
		error_printf("%s", "Failed to get ASCII target!");
	
	return 0; 
}
