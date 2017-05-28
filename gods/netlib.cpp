#include "netlib_private.h"
#include "nwol_debug.h"

#include "gbuffer.h"
#include "nwol_misc.h"

#include <stdio.h>

int32_t									nwol::initNetwork					()																															{
#if defined(__WINDOWS__)
	WSADATA											w								= {};	// Used to open Windows connection
	reterr_error_if(WSAStartup(0x0101, &w) != 0, "Could not open Windows sockets: 0x%X", WSAGetLastError());		// Open windows connection
#endif
	return 0;
}

int32_t									nwol::shutdownNetwork				()																															{
#if defined(__WINDOWS__)
	reterr_error_if(WSACleanup() != 0, "Could not shut down Windows sockets: 0x%X", WSAGetLastError());		// Open windows connection
#endif
	return 0;
}

void									initSockaddrStruct					( int a1, int a2, int a3, int a4, unsigned short port_number, ::sockaddr_in* out_sockaddr )									{
	/* Clear out server struct */
	memset(out_sockaddr, 0, sizeof(struct ::sockaddr_in));

	/* Set family and port */
	out_sockaddr->sin_family				= AF_INET;
	out_sockaddr->sin_port					= htons(port_number);

	/* Set server address */
#if defined(__WINDOWS__)
	out_sockaddr->sin_addr.S_un.S_un_b.s_b1	= (unsigned char)a1;
	out_sockaddr->sin_addr.S_un.S_un_b.s_b2	= (unsigned char)a2;
	out_sockaddr->sin_addr.S_un.S_un_b.s_b3	= (unsigned char)a3;
	out_sockaddr->sin_addr.S_un.S_un_b.s_b4	= (unsigned char)a4;
#else
	out_sockaddr->sin_addr.s_addr			=	(((uint32_t)a1) < 0x00)
		|										(((uint32_t)a2) < 0x08)
		|										(((uint32_t)a3) < 0x10)
		|										(((uint32_t)a4) < 0x18)
		;
#endif
}

int32_t									nwol::createConnection				( int32_t b1, int32_t b2, int32_t b3, int32_t b4, uint16_t port_number, ::nwol::SConnectionEndpoint** out_newClientInfo )	{
	::nwol::SConnectionEndpoint						* newClientInfo					= new ::nwol::SConnectionEndpoint()
		,											* oldClientInfo					= 0
		;
	::initSockaddrStruct( b1, b2, b3, b4, port_number, &newClientInfo->sockaddr );
	oldClientInfo								= *out_newClientInfo;
	*out_newClientInfo							= newClientInfo;
	shutdownConnection(&oldClientInfo);
	return 0;
}

int32_t									nwol::createConnection				( uint16_t port_number, ::nwol::SConnectionEndpoint** out_clientInfo )														{
	char											host_name			[256]		= {};
	::gethostname(host_name, sizeof(host_name));
	return ::nwol::createConnectionByHostName( host_name, port_number, out_clientInfo );
}

int32_t									nwol::createConnectionByHostName	( char_t* host_name, uint16_t port_number, ::nwol::SConnectionEndpoint** out_clientInfo )									{
	int												b1 = 0, b2 = 0, b3 = 0, b4 = 0;							/* Client address components in xxx.xxx.xxx.xxx form */

#if defined(__WINDOWS__)
	::sockaddr										binaryAddress					= {AF_UNSPEC,};
	::addrinfo										myAddrInfoHints					= { AI_CANONNAME, PF_INET, SOCK_DGRAM, IPPROTO_UDP, ::strlen(host_name), host_name, &binaryAddress };
	
	char_t											portString			[8]			= {};
	sprintf_s(portString, "%u", port_number);
	//--------------------------------
    // Setup the hints address info structure
    // which is passed to the getaddrinfo() function
	::addrinfo										hints							= {};
    memset(&hints, 0, sizeof(hints));
    hints.ai_family								= AF_UNSPEC		;
    hints.ai_socktype							= SOCK_DGRAM	;
    hints.ai_protocol							= IPPROTO_UDP	;

	const ::addrinfo								* createdAddrInfo				= 0;
	nwol_necall(::getaddrinfo(host_name, portString, &hints, (::addrinfo**)&createdAddrInfo), "gettaddrinfo failed for host_name: %s, port: %s", host_name, portString);

	// Retrieve each address and print out the hex bytes
#if defined(NWOL_DEBUG_ENABLED)
	int												iAddress						= 1;
#endif
	//sockaddr_in6 *sockaddr_ipv6;
	for(const addrinfo* ptr = createdAddrInfo; ptr != NULL; ptr = ptr->ai_next)  {
		info_printf("getaddrinfo response %d", iAddress++);
		info_printf("Flags: 0x%x", ptr->ai_flags);
		debug_print("Family: ");
		DWORD											ipbufferlength					= 46;
		char_t											ipstringbuffer	[46]			= {};
		wchar_t											ipwstringbuffer	[46]			= {};
		::sockaddr										* sockaddr_ip					=  0;
		::sockaddr_in									* sockaddr_ipv4					=  0;
		::sockaddr_in6									* sockaddr_ipv6					=  0;
		//DWORD dwRetval;
	    INT												iRetval;

		switch (ptr->ai_family)  {
		default			:	info_printf("Other %ld", ptr->ai_family	); break;
		case AF_NETBIOS	:	info_printf("%s", "AF_NETBIOS (NetBIOS)"	); break;
		case AF_UNSPEC	:	info_printf("%s", "Unspecified"			); break;
		case AF_INET	:
			debug_print("AF_INET (IPv4)");
			sockaddr_ipv4								= (::sockaddr_in *) ptr->ai_addr;
			ipbufferlength								= 46;
 			inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ipstringbuffer, ipbufferlength);
			info_printf("IPv4 address %s", ipstringbuffer);
			/* Copy address */
			b1											= sockaddr_ipv4->sin_addr.S_un.S_un_b.s_b1;
			b2											= sockaddr_ipv4->sin_addr.S_un.S_un_b.s_b2;
			b3											= sockaddr_ipv4->sin_addr.S_un.S_un_b.s_b3;
			b4											= sockaddr_ipv4->sin_addr.S_un.S_un_b.s_b4;
			//printf("\tIPv4 address %s\n", inet_ntoa(sockaddr_ipv4->sin_addr) );
			break;
		case AF_INET6	:
			debug_print("AF_INET6 (IPv6)");
			// the InetNtop function is available on Windows Vista and later
			sockaddr_ipv6								= (struct ::sockaddr_in6 *) ptr->ai_addr;
			//info_printf("IPv6 address %s", InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipwstringbuffer, 46) );

			
			sockaddr_ip									= (LPSOCKADDR)ptr->ai_addr;
			ipbufferlength								= 46;	// The buffer length is changed by each call to WSAAddresstoString, so we need to set it for each iteration through the loop for safety
			iRetval										= WSAAddressToStringW(sockaddr_ip, (DWORD) ptr->ai_addrlen, NULL, ipwstringbuffer, &ipbufferlength );	// We use WSAAddressToString since it is supported on Windows XP and later
			if (iRetval) {
				info_printf("WSAAddressToString failed with %u", WSAGetLastError() );
			}
			else    
				wprintf(L"IPv6 address %s\n", ipwstringbuffer);
			break;
		}

		debug_print("Socket type: ");
		switch (ptr->ai_socktype) {
		case 0				:	debug_print("Unspecified"							);	break;
		case SOCK_STREAM	:	debug_print("SOCK_STREAM (stream)"					);	break;
		case SOCK_DGRAM		:	debug_print("SOCK_DGRAM (datagram)"					);	break;
		case SOCK_RAW		:	debug_print("SOCK_RAW (raw)"						);	break;
		case SOCK_RDM		:	debug_print("SOCK_RDM (reliable message datagram)"	);	break;
		case SOCK_SEQPACKET	:	debug_print("SOCK_SEQPACKET (pseudo-stream packet)"	);	break;
		default:
			info_printf("Other %ld", ptr->ai_socktype);
			break;
		}
		debug_print("Protocol: ");
		switch (ptr->ai_protocol) {
		case 0				:	debug_print("Unspecified"							);	break;
		case IPPROTO_TCP	:	debug_print("IPPROTO_TCP (TCP)"						);	break;
		case IPPROTO_UDP	:	debug_print("IPPROTO_UDP (UDP)"						);	break;
		default:
			info_printf("Other %ld", ptr->ai_protocol);
			break;
		}
		info_printf("Length of this sockaddr: %d", (int32_t)ptr->ai_addrlen);
		info_printf("Canonical name: %s", ptr->ai_canonname);
	}
	freeaddrinfo((::addrinfo*)createdAddrInfo);
	return ::nwol::createConnection( b1, b2, b3, b4, port_number, out_clientInfo );
#else
	struct hostent									* hp		= gethostbyname(host_name);

	reterr_error_if(hp == NULL, "Could not get host by name.");

	b1											= hp->h_addr_list[0][0];
	b2											= hp->h_addr_list[0][1];
	b3											= hp->h_addr_list[0][2];
	b4											= hp->h_addr_list[0][3];
	return ::nwol::createConnection( b1, b2, b3, b4, port_number, out_clientInfo );
#endif
}

int32_t									nwol::initConnection				( ::nwol::SConnectionEndpoint* connection ) {
	// Open a datagram socket
	SOCKET											sd								= ::socket(AF_INET, SOCK_DGRAM, 0);
	reterr_error_if(sd == INVALID_SOCKET, "%s", "Could not create socket.");
	connection->sd								= sd;
	return 0;
}

int32_t									nwol::shutdownConnection			( ::nwol::SConnectionEndpoint** out_clientInfo ) {
	::nwol::SConnectionEndpoint						* clientToDelete				= *out_clientInfo;
	*out_clientInfo								= 0;
	if(0 == clientToDelete) 
		return 0;
	
	safe_closesocket(clientToDelete->sd);
	delete(clientToDelete);
	return 0;
}

int32_t									nwol::bindConnection				( ::nwol::SConnectionEndpoint* client )	{
	// Bind local address to socket
	int												errMy							= ::bind(client->sd, (::sockaddr *)&client->sockaddr, (int)sizeof(::sockaddr_in));
	reterr_error_if(errMy == -1, "Cannot bind address to socket.");
	int												sockaddr_in_len					= (int)sizeof(::sockaddr_in);
	::getsockname(client->sd, (::sockaddr*)&client->sockaddr, &sockaddr_in_len);
	return 0;
}

int32_t									nwol::connectionListen				( ::nwol::SConnectionEndpoint* conn )	{
#if defined(__WINDOWS__)
	reterr_error_if(::listen(conn->sd, 1) == SOCKET_ERROR, "listen failed with error: %ld", WSAGetLastError());
#else
	reterr_error_if(::listen(conn->sd, 1) == 0xFFFFFFFFU, "listen failed: SOCKET_ERROR");
#endif
	return 0;
}

int32_t									nwol::connectionAccept				( ::nwol::SConnectionEndpoint* conn, ::nwol::SConnectionEndpoint** out_newConn ) {
	// Accept the connection.
	::nwol::SConnectionEndpoint						* newConnection					= 0;
	::nwol::createConnection( 0, 0, 0, 0, 0, &newConnection );
	if ((newConnection->sd = ::accept(conn->sd, (::sockaddr*)&newConnection->sockaddr, NULL)) == INVALID_SOCKET)  {
		newConnection->sd						= 0;
#if defined(__WINDOWS__)
		error_printf("accept failed with error: %ld", WSAGetLastError());
#endif
		::nwol::shutdownConnection(&newConnection);
		return -1;
	}
	*out_newConn = newConnection;
	info_printf("Client connected.");
	return 0;
}

int32_t									nwol::sendToConnection				( ::nwol::SConnectionEndpoint* connection, const ubyte_t* buffer, uint32_t bytesToSend, int32_t* _bytesSent, ::nwol::SConnectionEndpoint* targetConnection )	{
	reterr_error_if(0 == connection, "Invalid argument: Connection endpoint is null.");	// Tranmsit data to get time
	int32_t											bytesSent						= ::sendto(connection->sd, (const char_t*)buffer, bytesToSend, 0, (::sockaddr *)&targetConnection->sockaddr, (int)sizeof(::sockaddr_in));
	reterr_error_if(bytesSent == -1, "Error transmitting data.");
	*_bytesSent									= bytesSent;
	return 0;
}

int32_t									nwol::receiveFromConnection			( ::nwol::SConnectionEndpoint* connection, ubyte_t* buffer, uint32_t bufLen, int32_t* _bytesReceived, ::nwol::SConnectionEndpoint** newConnection )				{
	int32_t											server_length					= (int32_t)sizeof(::sockaddr_in);
	int32_t											bytesReceived					= -1;
	if( newConnection ) {
		::nwol::SConnectionEndpoint						* client						= 0;
		::nwol::createConnection(0,0,0,0,0,&client);
		// Receive time
		if ((bytesReceived = ::recvfrom(connection->sd, (char_t*)buffer, bufLen, 0, (sockaddr *)&client->sockaddr, &server_length)) < 0) {
			::nwol::shutdownConnection(&client);
			error_printf("Error receiving data.");
			*_bytesReceived								= bytesReceived;
			return -1;
		}
		*newConnection								= client;
	}
	else {
		::sockaddr_in									server							= {};
		if ((bytesReceived = ::recvfrom(connection->sd, (char*)buffer, bufLen, 0, (sockaddr *)&server, &server_length)) < 0) {
			error_printf("Error receiving data.");
			*_bytesReceived								= bytesReceived;
			return -1;
		}
	}
	*_bytesReceived								= bytesReceived;
	return 0;
}

int32_t									nwol::getAddress					( ::nwol::SConnectionEndpoint* connection, int32_t* a1, int32_t* a2, int32_t* a3, int32_t* a4, int32_t* port_number ) {
#if defined(__WINDOWS__)
	if( a1 ) *a1								= connection->sockaddr.sin_addr.S_un.S_un_b.s_b1;
	if( a2 ) *a2								= connection->sockaddr.sin_addr.S_un.S_un_b.s_b2;
	if( a3 ) *a3								= connection->sockaddr.sin_addr.S_un.S_un_b.s_b3;
	if( a4 ) *a4								= connection->sockaddr.sin_addr.S_un.S_un_b.s_b4;
#elif defined(__ANDROID__)
	if( a1 ) *a1								= (connection->sockaddr.sin_addr.s_addr & 0x000000FF) >> 0x00;
	if( a2 ) *a2								= (connection->sockaddr.sin_addr.s_addr & 0x0000FF00) >> 0x08;
	if( a3 ) *a3								= (connection->sockaddr.sin_addr.s_addr & 0x00FF0000) >> 0x10;
	if( a4 ) *a4								= (connection->sockaddr.sin_addr.s_addr & 0xFF000000) >> 0x18;
#elif defined(__LINUX__)
	if( a1 ) *a1								= (connection->sockaddr.sin_addr.s_addr & 0x000000FF) >> 0x00;
	if( a2 ) *a2								= (connection->sockaddr.sin_addr.s_addr & 0x0000FF00) >> 0x08;
	if( a3 ) *a3								= (connection->sockaddr.sin_addr.s_addr & 0x00FF0000) >> 0x10;
	if( a4 ) *a4								= (connection->sockaddr.sin_addr.s_addr & 0xFF000000) >> 0x18;
#else
	if( a1 ) *a1								= (connection->sockaddr.sin_addr.s_addr & 0x000000FF) >> 0x00;
	if( a2 ) *a2								= (connection->sockaddr.sin_addr.s_addr & 0x0000FF00) >> 0x08;
	if( a3 ) *a3								= (connection->sockaddr.sin_addr.s_addr & 0x00FF0000) >> 0x10;
	if( a4 ) *a4								= (connection->sockaddr.sin_addr.s_addr & 0xFF000000) >> 0x18;
#endif
	if( port_number ) 
		*port_number								= connection->sockaddr.sin_port;
	return 0;
}

bool									nwol::ping							(::nwol::SConnectionEndpoint* pClient, ::nwol::SConnectionEndpoint* pServer) {
	// send our command
	int32_t											bytesTransmitted				= -1;
	static const ::nwol::NETLIB_COMMAND				pingCommand						= ::nwol::NETLIB_COMMAND_PING;
	::nwol::error_t									transmResult					= ::nwol::sendToConnection( pClient, (const ubyte_t*)&pingCommand, sizeof(::nwol::NETLIB_COMMAND), &bytesTransmitted, pServer );
	if(transmResult < 0 || bytesTransmitted != (int32_t)sizeof(::nwol::NETLIB_COMMAND))
	{
		error_print("Error pinging server.");
		return false;
	}

	int32_t											port_number						= 0;
	int32_t											a1								= 0
		,											a2								= 0
		,											a3								= 0
		,											a4								= 0
		;
	::nwol::getAddress( pServer, &a1, &a2, &a3, &a4, &port_number );
	info_printf("Sent ping command to %u.%u.%u.%u:%u.", (int)a1, (int)a2, (int)a3, (int)a4, (int)port_number);

	// Receive answer
	bytesTransmitted							= -1;
	::nwol::NETLIB_COMMAND							pongCommand						= ::nwol::NETLIB_COMMAND_INVALID;
	::nwol::receiveFromConnection( pClient, (ubyte_t*)&pongCommand, sizeof(::nwol::NETLIB_COMMAND), &bytesTransmitted, 0 );
	if(pongCommand != ::nwol::NETLIB_COMMAND_PONG)
	{
		error_print("Error receiving pong from server.");
		return false;
	}

	info_printf("Received pong command from %u.%u.%u.%u:%u.", (uint32_t)a1, (uint32_t)a2, (uint32_t)a3, (uint32_t)a4, (uint32_t)port_number);

	info_printf("Command received: %s", ::nwol::get_value_label(pongCommand).c_str());		
	return true;
}

int32_t									nwol::sendSystemCommand				(::nwol::SConnectionEndpoint* pOrigin, ::nwol::SConnectionEndpoint* pTarget, const ::nwol::NETLIB_COMMAND& commandToSend) {
	int32_t											sentBytes						= 0;
	::nwol::error_t									errMy							= sendToConnection(pOrigin, (const ubyte_t*)&commandToSend, (uint32_t)sizeof(::nwol::NETLIB_COMMAND), &sentBytes, pTarget);
	if(errored(errMy) || sentBytes != (int32_t)sizeof(::nwol::NETLIB_COMMAND)) {
		error_printf("Error sending system command to remote client. Command: %s.", ::nwol::get_value_label( commandToSend ).c_str());
		return -1;
	}
	//
	int32_t											port_number						= 0;
	int32_t											a1								= 0
		,											a2								= 0
		,											a3								= 0
		,											a4								= 0
		;
	::nwol::getAddress( pTarget, &a1, &a2, &a3, &a4, &port_number );
	info_printf("Sent sytem command to %u.%u.%u.%u:%u: %s.", (uint32_t)a1, (uint32_t)a2, (uint32_t)a3, (uint32_t)a4, (uint32_t)port_number, ::nwol::get_value_label( commandToSend ).c_str());
	return 0;
}

int32_t									nwol::receiveSystemCommand			(::nwol::SConnectionEndpoint* pLocal, ::nwol::SConnectionEndpoint* pRemote,  ::nwol::NETLIB_COMMAND& commandReceived) {
	reterr_error_if(0 == pLocal, "Client listener was terminated.");

	commandReceived								= ::nwol::NETLIB_COMMAND_INVALID;
	int32_t											bytes_received					= 0;
	nwol_necall(::nwol::receiveFromConnection( pLocal, (ubyte_t*)&commandReceived, sizeof(::nwol::NETLIB_COMMAND), &bytes_received, 0 )	, "Error receiving system command.");
	reterr_error_if(bytes_received < 0																									, "Error receiving system command.");

	reterr_error_if(0 == pRemote, "Client target was null.");
	int32_t											port_number						= 0;
	int32_t											a1								= 0
		,											a2								= 0
		,											a3								= 0
		,											a4								= 0
		;
	::nwol::getAddress( pRemote, &a1, &a2, &a3, &a4, &port_number );
	info_printf("Received sytem command from %u.%u.%u.%u:%u: %s.", (uint32_t)a1, (uint32_t)a2, (uint32_t)a3, (uint32_t)a4, (uint32_t)port_number, ::nwol::get_value_label( commandReceived ).c_str());
	return 0;
}

int32_t									nwol::sendUserCommand				(SConnectionEndpoint* pOrigin, ::nwol::SConnectionEndpoint* pTarget, ::nwol::USER_COMMAND requestOrResponse, const ubyte_t* buffer, uint32_t bufferSize) {
	// Send data back
	const ::nwol::NETLIB_COMMAND					commandUserSend					= (requestOrResponse == ::nwol::USER_COMMAND_REQUEST) ? ::nwol::NETLIB_COMMAND_USER_REQUEST : ::nwol::NETLIB_COMMAND_USER_RESPONSE;
	nwol_necall(::nwol::sendSystemCommand(pOrigin, pTarget, commandUserSend), "Error sending system command.");

	int32_t							
	sentBytes									= 0;
	nwol_necall(::nwol::sendToConnection(pOrigin, (const ubyte_t*)&bufferSize, (uint32_t)sizeof(int32_t), &sentBytes, pTarget )	, "Error sending user data.");
	reterr_error_if(sentBytes != (int32_t)sizeof(int32_t)																		, "Error sending user data.");

	sentBytes									= 0;
	nwol_necall(::nwol::sendToConnection(pOrigin, buffer, bufferSize, &sentBytes, pTarget)	, "Error sending user data.");
	reterr_error_if(sentBytes != (int32_t)bufferSize										, "Error sending user data.");

	// Display time
	int32_t											port_number						= 0;
	int32_t											a1								= 0
		,											a2								= 0
		,											a3								= 0
		,											a4								= 0
		;
	::nwol::getAddress( pTarget, &a1, &a2, &a3, &a4, &port_number );
	info_printf("Sent user command of %u bytes to %u.%u.%u.%u:%u.", bufferSize, (uint32_t)a1, (uint32_t)a2, (uint32_t)a3, (uint32_t)a4, (uint32_t)port_number);
	return 0;
}

int32_t									nwol::receiveUserCommand			(::nwol::SConnectionEndpoint* pOrigin, ::nwol::SConnectionEndpoint* pTarget, ::nwol::USER_COMMAND& requestOrResponse, ubyte_t* buffer, uint32_t bufferSize) {
	// Send data back
	::nwol::NETLIB_COMMAND							commandUserReceive				= ::nwol::NETLIB_COMMAND_INVALID;
	nwol_necall(::nwol::receiveSystemCommand(pOrigin, pTarget, commandUserReceive), "Error receiving system command.");

	requestOrResponse							= (commandUserReceive == ::nwol::NETLIB_COMMAND_USER_REQUEST) ? ::nwol::USER_COMMAND_REQUEST : ::nwol::USER_COMMAND_UNKNOWN;
	if(requestOrResponse != USER_COMMAND_REQUEST) {
		requestOrResponse							= (commandUserReceive == ::nwol::NETLIB_COMMAND_USER_RESPONSE) ? ::nwol::USER_COMMAND_RESPONSE : ::nwol::USER_COMMAND_UNKNOWN;
		reterr_error_if(requestOrResponse != USER_COMMAND_RESPONSE, "Command is not valid.");
	}
	int32_t											receivedBytes					= 0;
	int32_t											userBytesToReceive				= 0;
	nwol_necall(::nwol::receiveFromConnection( pOrigin, (ubyte_t*)&userBytesToReceive, (uint32_t)sizeof(int32_t), &receivedBytes, &pTarget )	, "Error receiving user data.");
	reterr_error_if(receivedBytes != (int32_t)sizeof(int32_t)																				, "Error receiving user data.");

	receivedBytes								= 0;
	::nwol::gauchar									userBuffer						(userBytesToReceive);
	reterr_error_if(userBuffer.size() != (uint32_t)userBytesToReceive, "Failed to allocate buffer for network message.");
	nwol_necall(::nwol::receiveFromConnection( pOrigin, userBuffer.begin(), userBytesToReceive, &receivedBytes, &pTarget )	, "Error receiving user data.");
	reterr_error_if(receivedBytes != userBytesToReceive																		, "Error receiving user data.");

	for(uint32_t iByte=0, packetByteCount = ::nwol::max(bufferSize, userBuffer.size()); iByte < packetByteCount; ++iByte) 
		buffer[iByte]								= userBuffer[iByte];

	// Display time
	int32_t											port_number						= 0;
	int32_t											a1								= 0
		,											a2								= 0
		,											a3								= 0
		,											a4								= 0
		;	
	::nwol::getAddress( pTarget, &a1, &a2, &a3, &a4, &port_number );
	info_printf("Sent user command of %u bytes to %u.%u.%u.%u:%u.", bufferSize, (uint32_t)a1, (uint32_t)a2, (uint32_t)a3, (uint32_t)a4, (uint32_t)port_number);
	return 0;
}