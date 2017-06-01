#include "nwoa.h"
#include "ascii_screen.h"

#include <chrono>
#include <thread>

int32_t														cleanup									(::SApplication& instanceApp)							{ 
	::nwol::shutdownASCIIScreen();

	nwol_necall(::networkDisable(instanceApp), "Error when disabling network.");
	::std::this_thread::sleep_for(::std::chrono::milliseconds(1000));
	return 0; 
}

//void														printTypeDebug							()														{
//	static const ::SApplication::TRegistry							memberRegistry							= ::SApplication::get_member_registry();
//	for(uint32_t iMember=0; iMember < memberRegistry.get_member_count(); ++iMember) {
//		const ::nwol::STypeIdentifier									& identifier							= memberRegistry.get_types()[iMember];
//		always_printf
//			(	"0x%*.8X: %s::%s %s (%s): %s"
//			,	8
//			,	memberRegistry.get_data_type_ids	()[iMember]
//			,	identifier.NameSpace	.c_str()
//			,	identifier.Name			.c_str()
//			,	memberRegistry.get_names			()[iMember].c_str()
//			,	memberRegistry.get_display_names	()[iMember].c_str()
//			,	memberRegistry.get_descriptions		()[iMember].c_str()
//			);	
//	}
//
//	::nwol::SApplicationNetworkClient::TRegistry					memberRegistryNet						= ::nwol::SApplicationNetworkClient::get_member_registry();
//	for(uint32_t iMember=0; iMember < memberRegistry.get_member_count(); ++iMember) {
//		const ::nwol::STypeIdentifier									& identifier							= memberRegistryNet.get_types()[iMember];
//		always_printf
//			(	"0x%*.8X: %s::%s %s (%s): %s"
//			,	8
//			,	memberRegistryNet.get_data_type_ids	()[iMember]
//			,	identifier.NameSpace	.c_str()
//			,	identifier.Name			.c_str()
//			,	memberRegistryNet.get_names			()[iMember].c_str()
//			,	memberRegistryNet.get_display_names	()[iMember].c_str()
//			,	memberRegistryNet.get_descriptions	()[iMember].c_str()
//			);
//	}
//}

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

int32_t														setup									(::SApplication& instanceApp)							{ 
	::nwol::SCoord2<uint32_t>										& sizeTargetASCII						= instanceApp.GUI.TargetSizeASCII;
	::nwol::initASCIIScreen(sizeTargetASCII.x, sizeTargetASCII.y);
	char															moduleTitle[240]						= {};
	uint8_t															moduleTitleLen							= (uint8_t)::nwol::size(moduleTitle);
	nwol_necall(::nwol_moduleTitle(moduleTitle, &moduleTitleLen), "If this fails then something weird is going on.");
	::nwol::setASCIIScreenTitle(moduleTitle);

	::nwol::setCooperativeLevels(instanceApp.RuntimeValues->Screen.PlatformDetail, instanceApp.MainScreenInput);	// This tells the input system that it has to bind to the main window.
	::nwol::acquireInput(instanceApp.MainScreenInput);

	//printTypeDebug();

	nwol_necall(::networkEnable	(instanceApp), "Failed to enable network."	);
	nwol_necall(::setupGUI		(instanceApp), "Failed to setup GUI."		);

	return 0; 
}
