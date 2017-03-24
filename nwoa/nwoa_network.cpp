#include "nwoa.h"

#include <thread>

#include <process.h>

int											runCommunications						(::nwol::SApplicationNetworkClient& appNetwork)	{
	::nwol::SClientConnection						& instanceClient						= appNetwork.Connection;

	::nwol::error_t									result									= 0;
	result										= ::nwol::initClientConnection	(instanceClient);	reterr_error_if_errored(result, "%s", "Failed to initialize client connection.");
	result										= ::nwol::connect				(instanceClient);	reterr_error_if_errored(result, "%s", "Failed to connect.");

	while gbit_true(appNetwork.State, ::nwol::NETWORK_STATE_ENABLED) {
		// Ping before anything else to make sure everything is more or less in order.
		if(false == ::nwol::ping(instanceClient.pClient, instanceClient.pServer))	{
			error_printf("%s", "Ping timeout.");
			result										= -1;
			break;
		}

		// get server time
		uint64_t										current_time;
		if errored(result = ::nwol::serverTime(instanceClient, current_time))		{
			error_printf("Failed to get server time. Error code: 0x%X.", (uint32_t)result);
			result										= -1;
			break;
		}
		
		{	// here we update the game instance with the data received from the server.
			::nwol::CLock									thelock									(appNetwork.ServerTimeMutex);
			appNetwork.ServerTime						= current_time;
			debug_printf("%s", "Client instance updated successfully.");
		}

		// Disconnect if the network was disabled.
		if gbit_false(appNetwork.State, ::nwol::NETWORK_STATE_ENABLED)
			break;

		::std::this_thread::sleep_for(::std::chrono::milliseconds(1000));
	}

	::nwol::requestDisconnect(instanceClient);

	gbit_clear(appNetwork.State, ::nwol::NETWORK_STATE_RUNNING);

	::nwol::disconnectClient(instanceClient);

	return result;
}

void										runCommunications						(void* pInstanceApp)						{
	debug_printf("Communications loop initializing.");
	if(0 == pInstanceApp)
		return;

	::SApplication									& instanceApp							= *(::SApplication*)pInstanceApp;
	::nwol::SApplicationNetworkClient				& instanceAppNetwork					= instanceApp.NetworkClient;

	const ::nwol::error_t							errMyComm								= ::runCommunications(instanceAppNetwork);

	gbit_clear(instanceAppNetwork.State, ::nwol::NETWORK_STATE_RUNNING);			
	debug_printf("Communications loop exited with code 0x%X.", errMyComm);
}

::nwol::error_t								networkEnable							(::SApplication& instanceApp)				{
	::nwol::error_t									errMy									= ::nwol::initNetwork();
	reterr_error_if_errored(errMy, "Failed to initialize network.");

	debug_printf("%s", "Network successfully initialized.");

	::nwol::SApplicationNetworkClient				& instanceAppNetwork					= instanceApp.NetworkClient;
	gbit_set(instanceAppNetwork.State, ::nwol::NETWORK_STATE_ENABLED);
	gbit_set(instanceAppNetwork.State, ::nwol::NETWORK_STATE_RUNNING);
	_beginthread(::runCommunications, 0, &instanceApp);
	::std::this_thread::sleep_for(::std::chrono::milliseconds(1000));
	debug_printf("%s", "Communications thread started.");

	return 0;
}

::nwol::error_t								networkDisable							(::SApplication& instanceApp)				{
	::nwol::SApplicationNetworkClient				& instanceAppNetwork					= instanceApp.NetworkClient;
	gbit_clear(instanceAppNetwork.State, ::nwol::NETWORK_STATE_ENABLED);
	::nwol::disconnectClient(instanceAppNetwork.Connection);
	
	while gbit_true(instanceAppNetwork.State, ::nwol::NETWORK_STATE_RUNNING)
		::std::this_thread::sleep_for(::std::chrono::milliseconds(1000));

	::nwol::shutdownNetwork();

	return 0;
}
