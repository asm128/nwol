#include "nwoa.h"

#include <thread>

#include <process.h>

int											runCommunications						(::nwol::SApplicationNetworkClient& appNetwork)	{
	::nwol::SClientConnection						& instanceClient						= appNetwork.Connection;
	nwol_pecall(::nwol::initClientConnection	(instanceClient), "%s", "Failed to initialize client connection.");
	nwol_pecall(::nwol::connect					(instanceClient), "%s", "Failed to connect.");

	::nwol::error_t									result									= 0;
	while gbit_true(appNetwork.State, ::nwol::NETWORK_STATE_ENABLED) {
		// Ping before anything else to make sure everything is more or less in order.
		break_error_if(result = ::nwol::ping(instanceClient.pClient, instanceClient.pServer) ? 0 : -1, "%s", "Ping timeout.");

		// get server time
		uint64_t										current_time;
		break_error_if(errored(result = ::nwol::serverTime(instanceClient, current_time)), "Failed to get server time.");
		
		{	// here we update the game instance with the data received from the server.
			::nwol::CLock									thelock									(appNetwork.ServerTimeMutex);
			appNetwork.ServerTime						= current_time;
			info_printf("%s", "Client instance updated successfully.");
		}

		break_info_if(gbit_false(appNetwork.State, ::nwol::NETWORK_STATE_ENABLED), "Disconnect as the network was disabled.");
		::std::this_thread::sleep_for(::std::chrono::milliseconds(1000));
	}

	::nwol::requestDisconnect(instanceClient);
	gbit_clear(appNetwork.State, ::nwol::NETWORK_STATE_RUNNING);
	::nwol::disconnectClient(instanceClient);

	return result;
}

void										runCommunications						(void* pInstanceApp)						{
	info_printf("Communications loop initializing.");
	ret_error_if(0 == pInstanceApp, "Invalid argument: SApplication* input parameter is null.")

	::SApplication									& instanceApp							= *(::SApplication*)pInstanceApp;
	::nwol::SApplicationNetworkClient				& instanceAppNetwork					= instanceApp.NetworkClient;

	const ::nwol::error_t							errMyComm								= ::runCommunications(instanceAppNetwork);

	gbit_clear(instanceAppNetwork.State, ::nwol::NETWORK_STATE_RUNNING);			
	info_printf("Communications loop exited with code 0x%X.", errMyComm);
}

::nwol::error_t								networkEnable							(::SApplication& instanceApp)				{
	nwol_pecall(::nwol::initNetwork(), "Failed to initialize network.");
	info_printf("%s", "Network successfully initialized.");

	::nwol::SApplicationNetworkClient				& instanceAppNetwork					= instanceApp.NetworkClient;
	gbit_set(instanceAppNetwork.State, ::nwol::NETWORK_STATE_ENABLED | ::nwol::NETWORK_STATE_RUNNING);
	_beginthread(::runCommunications, 0, &instanceApp);
	::std::this_thread::sleep_for(::std::chrono::milliseconds(1000));
	info_printf("%s", "Communications thread started.");
	return 0;
}

::nwol::error_t								networkDisable							(::SApplication& instanceApp)				{
	::nwol::SApplicationNetworkClient				& instanceAppNetwork					= instanceApp.NetworkClient;
	gbit_clear(instanceAppNetwork.State, ::nwol::NETWORK_STATE_ENABLED);
	//::nwol::disconnectClient(instanceAppNetwork.Connection);
	
	while gbit_true(instanceAppNetwork.State, ::nwol::NETWORK_STATE_RUNNING)
		::std::this_thread::sleep_for(::std::chrono::milliseconds(1000));

	::std::this_thread::sleep_for(::std::chrono::milliseconds(1000));
	::nwol::shutdownNetwork();
	return 0;
}
