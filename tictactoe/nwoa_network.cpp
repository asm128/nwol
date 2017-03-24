// The following code should be moved to a network client system that receives messages which should be pushed into a queue for the application to process on update()
#include "nwoa.h"

#include <thread>		

#if defined (__WINDOWS__)
#include <process.h>	// This is a Windows SDK header used for _beginthread().
#endif

// Loops sending and updating messages from/to the queues.
int											runCommunications						(::nwol::SApplicationNetworkClient& appNetwork)	{
	::nwol::SClientConnection						& instanceClient						= appNetwork.Connection;

	reterr_msg_if_error(::nwol::initClientConnection	(instanceClient), "Failed to initialize client connection.");
	reterr_msg_if_error(::nwol::connect					(instanceClient), "Failed to connect.");

	::nwol::error_t									result									= 0;
	while gbit_true(appNetwork.State, ::nwol::NETWORK_STATE_ENABLED) {

		if(false == ::nwol::ping(instanceClient.pClient, instanceClient.pServer))	{	// Ping before anything else to make sure everything is more or less in order.
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

		if gbit_false(appNetwork.State, ::nwol::NETWORK_STATE_ENABLED)	// Disconnect if the network was disabled.
			break;

		::std::this_thread::sleep_for(::std::chrono::milliseconds(1000));
	}

	::nwol::requestDisconnect(instanceClient);	// Finished communication loop. Request disconnect for the server to close the connection properly.

	gbit_clear(appNetwork.State, ::nwol::NETWORK_STATE_RUNNING);	// Clear flags to let others know that the loop ended.

	::nwol::disconnectClient(instanceClient);	// Close the connection client-side.

	return result;
}

// Proxy function to match the signature required by Windows' _beginthread().
void										runCommunications						(void* pInstanceAppNetwork)					{
	debug_printf("Communications loop initializing.");
	if(0 == pInstanceAppNetwork)
		return;

	::nwol::SApplicationNetworkClient				& instanceAppNetwork					= *(::nwol::SApplicationNetworkClient*)pInstanceAppNetwork;

	const ::nwol::error_t							errMyComm								= ::runCommunications(instanceAppNetwork);

	gbit_clear(instanceAppNetwork.State, ::nwol::NETWORK_STATE_RUNNING);			
	debug_printf("Communications loop exited with code 0x%X.", errMyComm);
}

::nwol::error_t								networkEnable							(::SApplication& instanceApp)				{
	::nwol::error_t									errMy									= ::nwol::initNetwork();
	reterr_msg_if_error(errMy, "Failed to initialize network. Error code: 0x%X.", (uint32_t)errMy);

	debug_printf("%s", "Network successfully initialized.");

	::nwol::SApplicationNetworkClient				& instanceAppNetwork					= instanceApp.NetworkClient;
	gbit_set(instanceAppNetwork.State, ::nwol::NETWORK_STATE_ENABLED);
	gbit_set(instanceAppNetwork.State, ::nwol::NETWORK_STATE_RUNNING);
	_beginthread(::runCommunications, 0, &instanceAppNetwork);
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
