#include "nwoa.h"

#include "ascii_screen.h"
#include "gui.h"

#include "nwol_runtime_impl.h"

#include <thread>
#include <process.h>

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(::SApplication, "No Workflow Overhead Application", 0, 1);

int32_t														update									(::SApplication& instanceApp, bool exitRequested)		{
	if(exitRequested)
		return ::nwol::APPLICATION_STATE_EXIT;

	::nwol::SInput													& consoleInputSystem					= instanceApp.Input;
	::nwol::SScreenInput											& mainScreeninputSystem					= instanceApp.MainScreenInput;
	::nwol::pollInput(consoleInputSystem);
	::nwol::pollInput(mainScreeninputSystem, instanceApp.RuntimeValues->Screen.PlatformDetail);

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
