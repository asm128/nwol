#include "nwoa.h"

#include "text.h"
#include "ascii_screen.h"
#include "gui.h"

#include "runtime_impl.h"

#include <thread>
#include <process.h>

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(::SApplication, "No Workflow Overhead Application", 0, 1);

int32_t														cleanup									(::SApplication& instanceApp)							{ 
	::nwol::shutdownASCIIScreen();

	nwol_necall(::networkDisable(instanceApp), "Error when disabling network.");
	::std::this_thread::sleep_for(::std::chrono::milliseconds(1000));
	return 0; 
}

void														printTypeDebug							()														{
	static const ::SApplication::TRegistry							memberRegistry							= ::SApplication::get_member_registry();
	for(uint32_t iMember=0; iMember < memberRegistry.get_member_count(); ++iMember) {
		const ::nwol::STypeIdentifier								& identifier						= memberRegistry.get_types()[iMember];
		always_printf
			(	"0x%*.8X: %s::%s %s (%s): %s"
			,	8
			,	memberRegistry.get_data_type_ids	()[iMember]
			,	identifier.NameSpace	.c_str()
			,	identifier.Name			.c_str()
			,	memberRegistry.get_names			()[iMember].c_str()
			,	memberRegistry.get_display_names	()[iMember].c_str()
			,	memberRegistry.get_descriptions		()[iMember].c_str()
			);	
	}

	::nwol::SApplicationNetworkClient::TRegistry					memberRegistryNet					= ::nwol::SApplicationNetworkClient::get_member_registry();
	for(uint32_t iMember=0; iMember < memberRegistry.get_member_count(); ++iMember) {
		const ::nwol::STypeIdentifier					& identifier						= memberRegistryNet.get_types()[iMember];
		always_printf
			(	"0x%*.8X: %s::%s %s (%s): %s"
			,	8
			,	memberRegistryNet.get_data_type_ids	()[iMember]
			,	identifier.NameSpace	.c_str()
			,	identifier.Name			.c_str()
			,	memberRegistryNet.get_names			()[iMember].c_str()
			,	memberRegistryNet.get_display_names	()[iMember].c_str()
			,	memberRegistryNet.get_descriptions	()[iMember].c_str()
			);
	}
}


int32_t														setupGUI								(::SApplication& instanceApp)							{ 
	::nwol::SGUI													& guiSystem								= instanceApp.GUI;

	guiSystem.TargetSizeASCII.x									= ::nwol::getASCIIBackBufferWidth	();
	guiSystem.TargetSizeASCII.y									= ::nwol::getASCIIBackBufferHeight	();

	static const ::nwol::glabel										newControlLabel							= "Exit";
	::nwol::SGUIControl												newControl;

	newControl.AreaASCII										= {1, 1, (int32_t)newControlLabel.size(), 1}	;
	newControl.Text												= newControlLabel								;
	nwol_necall(::nwol::createControl(guiSystem, newControl), "Failed to create control!");

	return 0;
}

::SApplication												* g_App									= nullptr;	// We need this for the WndProc()

int32_t														setup									(::SApplication& instanceApp)							{ 
	g_App														= &instanceApp;	// We need this for the WndProc()
	::nwol::SCoord2<uint32_t>										& sizeTargetASCII						= instanceApp.GUI.TargetSizeASCII;
	::nwol::initASCIIScreen(sizeTargetASCII.x, sizeTargetASCII.y);
	::nwol::setASCIIScreenTitle(nwol_moduleTitle());

	printTypeDebug();

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
