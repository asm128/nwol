#include "nwos.h"

#include "text.h"
#include "ascii_screen.h"
#include "netlib_server.h"

#include "runtime_impl.h"

#include <process.h>
#include <thread>

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(SApplication, "No Workflow Overhead Network Server", 0, 1);

bool														bListenFailure					= false;

::nwol::error_t												cleanup							(::SApplication& instanceApp)								{ 
	::nwol::shutdownASCIIScreen();

	instanceApp.NetworkServer.ShutdownServer();
	::nwol::shutdownNetwork();

	return 0; 
}

::nwol::error_t												serverInit						(::SApplication& instanceApp);
::nwol::error_t												setup							(::SApplication& instanceApp)								{ 
	::nwol::SGUI													& guiSystem						= instanceApp.GUI;
	
	::nwol::initASCIIScreen		(guiSystem.TargetSizeASCII.x, guiSystem.TargetSizeASCII.y);
	char															moduleTitle[240]				= {};
	uint8_t															moduleTitleLen					= (uint8_t)::nwol::size(moduleTitle);
	nwol_necall(::nwol_moduleTitle(moduleTitle, &moduleTitleLen), "If this fails then something weird is going on.");
	::nwol::setASCIIScreenTitle	(moduleTitle);

	guiSystem.TargetSizeASCII.x									= nwol::getASCIIBackBufferWidth		();
	guiSystem.TargetSizeASCII.y									= nwol::getASCIIBackBufferHeight	();

	static const ::nwol::glabel										newControlLabel					= "Exit";
	::nwol::SGUIControl												newControl;
	
	newControl.AreaASCII										= {1, 1, (int32_t)newControlLabel.size(), 1}	;
	newControl.Text												= newControlLabel								;

	nwol_necall(::nwol::createControl(instanceApp.GUI, newControl), "Failed to create GUI control %s.", newControl.Text.c_str());
	nwol_necall(::serverInit(instanceApp), "Failed to initialize server");
	return 0; 
}

#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif

::nwol::error_t												update									(::SApplication& instanceApp, bool exitRequested)											{
	if(exitRequested)
		return ::nwol::APPLICATION_STATE_EXIT;

	::nwol::SInput													& inputSystem							= instanceApp.Input;	nwol_necall(::nwol::pollInput(inputSystem), "Failed to poll input device.");
	::nwol::SGUI													& guiSystem								= instanceApp.GUI;		nwol_necall(::nwol::updateGUI(guiSystem, inputSystem), "Failed to update GUI states!");

	::nwol::array_pod<::nwol::CONTROL_FLAG>							& controlFlags							= guiSystem.Controls.ControlFlags;
	for(uint32_t iControl = 0, controlCount = controlFlags.size(); iControl < controlCount; ++iControl)
		if(::nwol::bit_true(controlFlags[iControl], ::nwol::CONTROL_FLAG_EXECUTE)) {
			info_printf("Execute %u.", iControl);
			switch(iControl) {
			case 0:		return ::nwol::APPLICATION_STATE_EXIT; 
			default:	break;
			}
		}
	return 0; 
}

int32_t														render									(::SApplication& instanceApp)																{ 
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

namespace nwol
{
	GDEFINE_ENUM_TYPE(SERVER_COMMAND, int16_t);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, LOGIN				,	 0);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, LOGIN_SUCCESS		,	 1);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, LOGIN_FAIL			,	 2);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, REGISTER				,	 3);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, REGISTER_SUCCESS		,	 4);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, REGISTER_FAIL		,	 5);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, TAKEN_USERNAME		,	 6);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, PLAYER_GET			,	 7);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, PLAYER_SET			,	 8);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, BUY					,	 9);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, SELL					,	10);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, RENAME				,	11);
	GDEFINE_ENUM_VALUE(SERVER_COMMAND, RESERVED				,	12);
}

void										serverListen					(::nwol::CServer* pServer)																							{
	while( !bListenFailure ) {
		error_if(bListenFailure = errored(pServer->Listen()), "Failed to listen on server")
		else if errored(pServer->Accept()) {
			error_print("Failed to accept queued client or no client queued.");
			::std::this_thread::sleep_for(::std::chrono::milliseconds(400));
		}
		::std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void										serverListen					( void* server )																									{ serverListen((::nwol::CServer*)server);	}
::nwol::error_t								serverInit						(::SApplication& instanceApp)																						{
	int												port_number;				// Port number to use
	char											port_number_str	[]				= "45678";
	reterr_error_if(sscanf_s(&port_number_str[0], "%u", &port_number) != 1, "%s.", "Invalid server port string.");	// Only run if port is provided

	nwol_necall(::nwol::initNetwork()								, "%s", "Failed to initialize network."			);	info_printf("%s initialized successfully.", "Network"			);
	nwol_necall(instanceApp.NetworkServer.InitServer(port_number)	, "%s", "Failed to initialize connection server.");	info_printf("%s initialized successfully.", "Connection server"	);
	_beginthread( serverListen, 0, &instanceApp.NetworkServer );	
	return 0;
}

::nwol::error_t								processCommand8					(uint8_t )																											{ return 0; }
::nwol::error_t								processCommand16				(uint16_t)																											{ return 0; }
::nwol::error_t								processCommand32				(uint32_t)																											{ return 0; }
::nwol::error_t								processCommand64				(uint64_t)																											{ return 0; }


::nwol::error_t								executeCommand					(::nwol::CClient* client, const char* payloadBuffer, uint32_t bufferLen)											{
	int												port_number;
	int												a1
		,											a2
		,											a3
		,											a4
		;
	::nwol::getAddress(client->ClientTarget, &a1, &a2, &a3, &a4, &port_number);
	switch(bufferLen) {
	case 1:	{	uint8_t		receivedValue = 0xFF				; memcpy(&receivedValue, payloadBuffer, bufferLen); return processCommand8	(receivedValue); break; }
	case 2:	{	uint16_t	receivedValue = 0xFFFF				; memcpy(&receivedValue, payloadBuffer, bufferLen); return processCommand16	(receivedValue); break; }
	case 4:	{	uint32_t	receivedValue = 0xFFFFFFFF			; memcpy(&receivedValue, payloadBuffer, bufferLen); return processCommand32	(receivedValue); break; }
	case 8:	{	uint64_t	receivedValue = 0xFFFFFFFFFFFFFFFF	; memcpy(&receivedValue, payloadBuffer, bufferLen); return processCommand64	(receivedValue); break; }
	default:
		if (strcmp(payloadBuffer, "GET PLAYER\r\n") == 0) {
			::nwol::SERVER_COMMAND							response						= ::nwol::SERVER_COMMAND_INVALID;							
			uint32_t										bytesToSend						= sizeof(response);

			error_if(errored(::nwol::sendUserCommand(client->ClientListener, client->ClientTarget, ::nwol::USER_COMMAND_RESPONSE, (const uint8_t*)&response, bytesToSend))
					, "Failed to send player data to %u.%u.%u.%u:%u."
					, (uint32_t)a1
					, (uint32_t)a2
					, (uint32_t)a3
					, (uint32_t)a4
					, (uint32_t)port_number
				)
			else {
				//info_printf("Sent player data (%s) to %u.%u.%u.%u:%u.", player.Name.c_str()
				//	, (int)a1
				//	, (int)a2
				//	, (int)a3
				//	, (int)a4
				//	, (int)port_number
				//	);
			}
		}
		else {
			::nwol::SERVER_COMMAND							response						= ::nwol::SERVER_COMMAND_INVALID;							
			uint32_t										bytesToSend						= sizeof(response);

			error_if(errored(::nwol::sendUserCommand(client->ClientListener, client->ClientTarget, ::nwol::USER_COMMAND_RESPONSE, (const uint8_t*)&response, bytesToSend))
					, "Failed to send player data to %u.%u.%u.%u:%u."
					,	(uint32_t)a1
					,	(uint32_t)a2
					,	(uint32_t)a3
					,	(uint32_t)a4
					,	(uint32_t)port_number
					)
			else {
				info_printf("Sent invalid message response to %u.%u.%u.%u:%u."
					,	(uint32_t)a1
					,	(uint32_t)a2
					,	(uint32_t)a3
					,	(uint32_t)a4
					,	(uint32_t)port_number
					);
			}
		}
	}


	return 0;
}