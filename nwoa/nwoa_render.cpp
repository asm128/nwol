#include "nwoa.h"
#include "ascii_screen.h"

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
