#include "nwos.h"

#include "text.h"
#include "ascii_screen.h"
#include "netlib_server.h"

#include "runtime_impl.h"

#include <process.h>
#include <thread>

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(SApplication, "No Workflow Overhead Network Server", 0, 1);

bool										bListenFailure					= false;

::nwol::error_t								cleanup							(::SApplication& instanceApp)								{ 
	::nwol::shutdownASCIIScreen();

	instanceApp.NetworkServer.ShutdownServer();
	::nwol::shutdownNetwork();

	return 0; 
}

::nwol::error_t								serverInit						(::SApplication& instanceApp);
::nwol::error_t								setup							(::SApplication& instanceApp)								{ 
	::nwol::SGUI									& guiSystem						= instanceApp.GUI;
	
	::nwol::initASCIIScreen		(guiSystem.TargetSizeASCII.x, guiSystem.TargetSizeASCII.y);
	::nwol::setASCIIScreenTitle	(appTitle());

	guiSystem.TargetSizeASCII.x					= nwol::getASCIIBackBufferWidth		();
	guiSystem.TargetSizeASCII.y					= nwol::getASCIIBackBufferHeight	();

	static const ::nwol::glabel						newControlLabel					= "Exit";
	::nwol::SGUIControl								newControl;
	
	newControl.AreaASCII						= {1, 1, (int32_t)newControlLabel.size(), 1}	;
	newControl.Text								= newControlLabel								;

	::nwol::createControl(instanceApp.GUI, newControl);

	::serverInit(instanceApp);
	return 0; 
}

#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif

::nwol::error_t								update							(::SApplication& instanceApp, bool exitRequested)			{
	if(exitRequested)
		return ::nwol::APPLICATION_STATE_EXIT;

	::nwol::SInput									& inputSystem					= instanceApp.Input;
	::nwol::pollInput(inputSystem);
	//if(inputSystem.Keys[VK_ESCAPE])
	//	return ::nwol::APPLICATION_STATE_EXIT;

	::nwol::SGUI									& guiSystem						= instanceApp.GUI;
	::nwol::updateGUI(guiSystem, inputSystem);

	::nwol::array_pod<::nwol::CONTROL_FLAG>			& controlFlags					= guiSystem.Controls.ControlFlags;
	for(uint32_t iControl = 0, controlCount = controlFlags.size(); iControl < controlCount; ++iControl)
		if(::nwol::bit_true(controlFlags[iControl], ::nwol::CONTROL_FLAG_EXECUTE)) {
			debug_printf("Execute %u.", iControl);
			switch(iControl) {
			case 0:		return ::nwol::APPLICATION_STATE_EXIT; 
			default:	break;
			}
		}
	return 0; 
}

::nwol::error_t								render							(::SApplication& instanceApp)								{ 
	::nwol::clearASCIIBackBuffer(' ', COLOR_WHITE);

	::nwol::SASCIITarget							target;
	::nwol::error_t									errMy							= ::nwol::getASCIIBackBuffer(target);
	reterr_error_if_errored(errMy, "%s", "Failed to get ASCII target!");
	
	::nwol::renderGUIASCII(target, instanceApp.GUI);

	::nwol::presentASCIIBackBuffer();
	
	return 0; 
}

namespace nwol
{
	GDEFINE_ENUM_TYPE(SERVER_COMMAND, int16_t);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND,	 0, LOGIN				);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND,	 1, LOGIN_SUCCESS		);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND,	 2, LOGIN_FAIL			);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND,	 3, REGISTER			);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND,	 4, REGISTER_SUCCESS	);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND,	 5, REGISTER_FAIL		);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND,	 6, TAKEN_USERNAME		);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND,	 7, PLAYER_GET			);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND,	 8, PLAYER_SET			);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND,	 9, BUY					);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND,	10, SELL				);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND,	11, RENAME				);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND,	12, RESERVED			);
}

void										serverListen					(::nwol::CServer* pServer)									{
	while( !bListenFailure ) {
		if( pServer->Listen() ) {
			error_print("Failed to listen on server.");
			bListenFailure																	= true;
		}
		else if( 0 > pServer->Accept() ) {
			error_print("Failed to accept queued client or no client queued.");
			::std::this_thread::sleep_for(::std::chrono::milliseconds(400));
		}

		::std::this_thread::sleep_for(std::chrono::milliseconds(100));
		//WriteTitle( pServer->ClientConnections.size() );
	}
}


void										serverListen					( void* server )											{ serverListen((::nwol::CServer*)server);	}
::nwol::error_t								serverInit						(::SApplication& instanceApp) {
	int												port_number;				// Port number to use
	char											port_number_str	[]				= "45678";

	reterr_error_if(sscanf_s(&port_number_str[0], "%u", &port_number) != 1	, "%s.", "Invalid server port string.");	// Only run if port is provided

	// Open windows connection 
	reterr_msg_if_error(::nwol::initNetwork()								, "%s.", "Failed to initialize network.");
	reterr_msg_if_error(instanceApp.NetworkServer.InitServer(port_number)	, "%s.", "Failed to initialize connection server.");

	_beginthread( serverListen, 0, &instanceApp.NetworkServer );	// Print out server information
	return 0;
}


::nwol::error_t								executeCommand					(::nwol::CClient* client, const char* buffer)
{
	uint64_t										current_time					= time(0);	// Get current time
	int												port_number;
	int												a1
		,											a2
		,											a3
		,											a4
		;
	::nwol::getAddress( client->m_ClientTarget, &a1, &a2, &a3, &a4, &port_number );

	if (strcmp(buffer, "GET PLAYER\r\n") == 0) {
		//klib::SPlayer player;
		if(::nwol::sendUserCommand(client->m_ClientListener, client->m_ClientTarget, ::nwol::USER_COMMAND_RESPONSE, (const uint8_t*)&current_time, (uint32_t)sizeof(current_time))) {
			error_printf("Failed to send player data to %u.%u.%u.%u:%u.", 
				(uint32_t)a1,
				(uint32_t)a2,
				(uint32_t)a3,
				(uint32_t)a4,
				(uint32_t)port_number
			);
		}
		//debug_printf("Sent player data (%s) to %u.%u.%u.%u:%u.", player.Name.c_str(), 
		//	(int)a1,
		//	(int)a2,
		//	(int)a3,
		//	(int)a4,
		//	(int)port_number
		//);
	}
	else {
		
		const char										mypong	[]						= "INVALIDMSG\r\n";									// Get current time
		uint32_t										bytesToSend						= ::nwol::size(mypong) * (uint32_t)sizeof(char_t);	// Send data back

		if(::nwol::sendUserCommand(client->m_ClientListener, client->m_ClientTarget, ::nwol::USER_COMMAND_RESPONSE, (const uint8_t*)&mypong, bytesToSend)) {
			error_printf("Failed to send player data to %u.%u.%u.%u:%u."
				,	(uint32_t)a1
				,	(uint32_t)a2
				,	(uint32_t)a3
				,	(uint32_t)a4
				,	(uint32_t)port_number
				);
		}

		debug_printf("Sent invalid message response to %u.%u.%u.%u:%u."
			,	(uint32_t)a1
			,	(uint32_t)a2
			,	(uint32_t)a3
			,	(uint32_t)a4
			,	(uint32_t)port_number
			);
	}

	return 0;
}