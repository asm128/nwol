#include "netlib_private.h"
#include "nwol_debug.h"

#include "gbuffer.h"
#include "nwol_misc.h"

#include "gref_definition.h"
#include "glist.h"

#include <stdio.h>

GDEFINE_PURE(gdnet, IEndpoint);
GDEFINE_OBJ(gdnet, SEndpoint);

::nwol::error_t								gdnet::endpointSystemInitialize					()																															{
#if defined(__WINDOWS__)
	::WSADATA										w												= {};	
	ree_if(::WSAStartup(0x0202, &w) != 0, "Could not open Windows sockets: 0x%X '%s'", WSAGetLastError(), ::nwol::getWindowsErrorAsString(WSAGetLastError()).c_str());		
#endif
	return 0;
}

::nwol::error_t								gdnet::endpointSystemShutdown					()																															{
#if defined(__WINDOWS__)
	ree_if(::WSACleanup() != 0, "Could not shut down Windows sockets: 0x%X '%s'", WSAGetLastError(), ::nwol::getWindowsErrorAsString(WSAGetLastError()).c_str());		// Open windows connection
#endif
	return 0;
}

void										initSockaddrStruct								(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint16_t port_number, ::sockaddr_in* out_sockaddr)												{
	*out_sockaddr								= {};	// Clear out server struct 

	// --- Set family and port 
	out_sockaddr->sin_family					= AF_INET;	
	out_sockaddr->sin_port						= htons(port_number);

	// --- Set server address 
#if defined(__WINDOWS__)
	out_sockaddr->sin_addr.S_un.S_un_b.s_b1		= (unsigned char)b1;
	out_sockaddr->sin_addr.S_un.S_un_b.s_b2		= (unsigned char)b2;
	out_sockaddr->sin_addr.S_un.S_un_b.s_b3		= (unsigned char)b3;
	out_sockaddr->sin_addr.S_un.S_un_b.s_b4		= (unsigned char)b4;
#else
	out_sockaddr->sin_addr.s_addr				=	(((uint32_t)b1) < 0x00)
		|											(((uint32_t)b2) < 0x08)
		|											(((uint32_t)b3) < 0x10)
		|											(((uint32_t)b4) < 0x18)
		;
#endif
}

::nwol::error_t								gdnet::endpointCreate							(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint16_t port_number, TRANSPORT_PROTOCOL mode, ::gdnet::GODS(IEndpoint)* out_endpointCreated)						{
	GPObj(::gdnet, SEndpoint)						endpointCreated									= {};
	::initSockaddrStruct(b1, b2, b3, b4, port_number, &endpointCreated->SockAddr);
	endpointCreated->Protocol						= mode;
	::nwol::set(out_endpointCreated, (::gdnet::GODS(IEndpoint))endpointCreated.get_core_ref());
	return 0;
}

::nwol::error_t								gdnet::endpointCreate							(uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, ::gdnet::GODS(IEndpoint)* out_endpointCreated)												{
	char											szHostName			[256]						= {};
	::gethostname(szHostName, sizeof(szHostName));
	return ::gdnet::endpointCreate(szHostName, portRequested, adapterIndex, mode, out_endpointCreated);
}

::nwol::error_t								gdnet::endpointInit								( ::gdnet::IEndpoint* endpoint )																																		{
	// Open a datagram socket
	::gdnet::SEndpoint								* implEndpoint									= (::gdnet::SEndpoint*)endpoint;
	SOCKET											sd												= ::socket(AF_INET, implEndpoint->Protocol ? SOCK_STREAM : SOCK_DGRAM, 0);
	ree_if(sd == INVALID_SOCKET, "%s", "Could not create socket.");
	implEndpoint->Socket							= sd;
	return 0;
}

::nwol::error_t								gdnet::endpointBind								( ::gdnet::IEndpoint* endpoint )																																		{
	::gdnet::SEndpoint								* implEndpoint									= (::gdnet::SEndpoint*)endpoint;
	int												errMy											= ::bind(implEndpoint->Socket, (::sockaddr *)&implEndpoint->SockAddr, (int)sizeof(::sockaddr_in));	// Bind address to socket
	ree_if(errMy == -1, "Cannot bind address to socket.");
	int												sockaddr_in_len									= (int)sizeof(::sockaddr_in);
	::getsockname(implEndpoint->Socket, (::sockaddr*)&implEndpoint->SockAddr, &sockaddr_in_len);
	return 0;
}

::nwol::error_t								gdnet::endpointListen							( ::gdnet::IEndpoint* endpoint )																																		{
	::gdnet::SEndpoint								* implEndpoint									= (::gdnet::SEndpoint*)endpoint;
#if defined(__WINDOWS__)
	ree_if(::listen(implEndpoint->Socket, 1) == SOCKET_ERROR, "listen failed with error: %ld", WSAGetLastError());
#else
	ree_if(::listen(implEndpoint->Socket, 1) == -1, "listen failed: SOCKET_ERROR");
#endif
	return 0;
}

::nwol::error_t								gdnet::endpointAccept							( ::gdnet::IEndpoint* endpointListener, ::gdnet::GODS(IEndpoint)* out_newConn )																						{
	GPNCO(::gdnet, IEndpoint)						acceptedEndpoint								= 0;
	::gdnet::endpointCreate(0, 0, TRANSPORT_PROTOCOL_TCP, &acceptedEndpoint);
	::gdnet::SEndpoint								* implAccepted									= (::gdnet::SEndpoint*)acceptedEndpoint.get_address();
	::gdnet::SEndpoint								* implListener									= (::gdnet::SEndpoint*)endpointListener;
	ree_if((implAccepted->Socket = ::accept(implListener->Socket, (::sockaddr*)&implAccepted->SockAddr, NULL)) == INVALID_SOCKET, "accept failed with error: %u. '%s'", (uint32_t)WSAGetLastError(), ::nwol::getWindowsErrorAsString(WSAGetLastError()).c_str());
	::nwol::set(out_newConn, acceptedEndpoint.get_core_ref());
	info_printf("Client connected.");
	return 0;
}

int32_t										gdnet::endpointSend								(::gdnet::IEndpoint* endpointLocal, const byte_t* buffer, uint32_t bytesToSend, int32_t* _bytesSent, ::gdnet::IEndpoint* endpointRemote)								{
	*_bytesSent									= 0;
	ree_if(0 == endpointLocal, "Invalid argument: Local endpoint is null.");	// Tranmsit data to get time
	::gdnet::SEndpoint								* implLocal										= (::gdnet::SEndpoint*)endpointLocal;
	::gdnet::SEndpoint								* implRemote									= (::gdnet::SEndpoint*)endpointRemote;
	int32_t											bytesSent										= ::sendto(implLocal->Socket, buffer, bytesToSend, 0, (::sockaddr *)&implRemote->SockAddr, (int)sizeof(::sockaddr_in));
	ree_if(bytesSent == -1, "Error transmitting data.");
	*_bytesSent									= bytesSent;
	return 0;
}

int32_t										gdnet::endpointReceive							(::gdnet::IEndpoint* endpointLocal, byte_t* buffer, uint32_t bufLen, int32_t* _bytesReceived, RECEIVE_FLAG receiveFlag, ::gdnet::GODS(IEndpoint)* endpointRemote)		{
	int32_t											server_length									= (int32_t)sizeof(::sockaddr_in);
	int32_t											bytesReceived									= -1;
	::gdnet::SEndpoint								* implLocal										= (::gdnet::SEndpoint*)endpointLocal;
	::sockaddr_in									addrRemote										= {};
	::sockaddr										* pAddrRemote									= {};
	GPNCO(::gdnet, IEndpoint)						newEndpointRemote								= {};
	if( endpointRemote ) {
		::gdnet::endpointCreate(0, 0, implLocal->Protocol, &newEndpointRemote);
		pAddrRemote									= (::sockaddr*)&((::gdnet::SEndpoint*)newEndpointRemote.get_address())->SockAddr;
	}
	else {
		pAddrRemote									= (::sockaddr*)&addrRemote;
	}
	if ((bytesReceived = ::recvfrom(implLocal->Socket, buffer, bufLen, (receiveFlag == ::gdnet::RECEIVE_FLAG_PEEK) ? MSG_PEEK : 0, pAddrRemote, &server_length)) < 0) {
		if(receiveFlag != ::gdnet::RECEIVE_FLAG_PEEK || ::WSAGetLastError() !=  WSAEMSGSIZE) {
			newEndpointRemote							= 0;
			switch(::WSAGetLastError()) {
			default		: error_printf("Error receiving data."); break;
			case 0x2714	:
				info_printf("Blocking operation canceled. This is normal when the socket was closed.");
				*_bytesReceived								 = 0;
				return 1;
			case 0x2746	:
				info_printf("Connection closed by the remote host.");
				*_bytesReceived								 = 0;
				return 1;
			}
		}
		else
			bytesReceived								= bufLen;
	}
	if(endpointRemote)
		::nwol::set(endpointRemote, newEndpointRemote.get_core_ref());

	safe_assign(_bytesReceived, bytesReceived);
	return bytesReceived;
}

int32_t										gdnet::endpointAddress							(::gdnet::IEndpoint* endpoint, uint8_t* a1, uint8_t* a2, uint8_t* a3, uint8_t* a4, uint16_t* port_number)								{
	rew_if(0 == endpoint, "Invalid input endpoint.")
	::gdnet::SEndpoint								* implEndpoint									= (::gdnet::SEndpoint*)endpoint;
#if defined(__WINDOWS__)
	if( a1 ) *a1									= implEndpoint->SockAddr.sin_addr.S_un.S_un_b.s_b1;
	if( a2 ) *a2									= implEndpoint->SockAddr.sin_addr.S_un.S_un_b.s_b2;
	if( a3 ) *a3									= implEndpoint->SockAddr.sin_addr.S_un.S_un_b.s_b3;
	if( a4 ) *a4									= implEndpoint->SockAddr.sin_addr.S_un.S_un_b.s_b4;
#elif defined(__ANDROID__)
	if( a1 ) *a1									= (implEndpoint->SockAddr.sin_addr.s_addr & 0x000000FF) >> 0x00;
	if( a2 ) *a2									= (implEndpoint->SockAddr.sin_addr.s_addr & 0x0000FF00) >> 0x08;
	if( a3 ) *a3									= (implEndpoint->SockAddr.sin_addr.s_addr & 0x00FF0000) >> 0x10;
	if( a4 ) *a4									= (implEndpoint->SockAddr.sin_addr.s_addr & 0xFF000000) >> 0x18;
#elif defined(__LINUX__)
	if( a1 ) *a1									= (implEndpoint->SockAddr.sin_addr.s_addr & 0x000000FF) >> 0x00;
	if( a2 ) *a2									= (implEndpoint->SockAddr.sin_addr.s_addr & 0x0000FF00) >> 0x08;
	if( a3 ) *a3									= (implEndpoint->SockAddr.sin_addr.s_addr & 0x00FF0000) >> 0x10;
	if( a4 ) *a4									= (implEndpoint->SockAddr.sin_addr.s_addr & 0xFF000000) >> 0x18;
#else
	if( a1 ) *a1									= (implEndpoint->SockAddr.sin_addr.s_addr & 0x000000FF) >> 0x00;
	if( a2 ) *a2									= (implEndpoint->SockAddr.sin_addr.s_addr & 0x0000FF00) >> 0x08;
	if( a3 ) *a3									= (implEndpoint->SockAddr.sin_addr.s_addr & 0x00FF0000) >> 0x10;
	if( a4 ) *a4									= (implEndpoint->SockAddr.sin_addr.s_addr & 0xFF000000) >> 0x18;
#endif
	if( port_number ) 
		*port_number									= ntohs(implEndpoint->SockAddr.sin_port);
	return 0;
}

::nwol::error_t								gdnet::endpointAddress							(const char_t* szHostName, uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, uint8_t* a1, uint8_t* a2, uint8_t* a3, uint8_t* a4)				{
	char_t											portString			[8]							= {};
	::sprintf_s(portString, "%u", portRequested);
	
	// Setup the hints address info structure which is passed to the getaddrinfo() function
	::addrinfo										hints											= {};
	::memset(&hints, 0, sizeof(hints));
	hints.ai_family								= AF_INET;
	hints.ai_socktype							= mode ? SOCK_STREAM	: SOCK_DGRAM	;
	hints.ai_protocol							= mode ? IPPROTO_TCP	: IPPROTO_UDP	;

	const ::addrinfo								* createdAddrInfo								= 0;
	ree_if(errored(::getaddrinfo(szHostName, portString, &hints, (::addrinfo**)&createdAddrInfo)), "gettaddrinfo failed for host_name: %s, port: %s", szHostName, portString);
	//sockaddr_in6									* sockaddr_ipv6									= 0;

	uint32_t										iAddress										= 0;
	iAddress;
	bool											addressFound									= false;
	for(const addrinfo* ptr = createdAddrInfo; ptr != NULL; ptr = ptr->ai_next)  {	// Retrieve each address and print out the hex bytes
		info_printf("getaddrinfo response at index %u.", iAddress);
		info_printf("Flags: 0x%x.", ptr->ai_flags);
		info_printf("Family: ");
		DWORD											ipbufferlength									= 46;
		char_t											ipstringbuffer	[46]							= {};
#if defined(__WINDOWS__)
		wchar_t											ipwstringbuffer	[46]							= {};
#endif
		::sockaddr										* sockaddr_ip									=  0;
		::sockaddr_in									* sockaddr_ipv4									=  0;
		::sockaddr_in6									* sockaddr_ipv6									=  0;
		//DWORD dwRetval;
		INT												iRetval;

		switch (ptr->ai_family)  {
		default			:	info_printf("Other %li.", ptr->ai_family	); break;
		case AF_NETBIOS	:	info_printf("%s", "AF_NETBIOS (NetBIOS)"	); break;
		case AF_UNSPEC	:	info_printf("%s", "Unspecified"				); break;
		case AF_INET	:
			info_printf("AF_INET (IPv4)");
			sockaddr_ipv4								= (::sockaddr_in *) ptr->ai_addr;
			ipbufferlength								= 46;
 			::inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ipstringbuffer, ipbufferlength);
			info_printf("IPv4 address %s", ipstringbuffer);
			// Copy address
			if(adapterIndex == iAddress) {
				*a1											= sockaddr_ipv4->sin_addr.S_un.S_un_b.s_b1;
				*a2											= sockaddr_ipv4->sin_addr.S_un.S_un_b.s_b2;
				*a3											= sockaddr_ipv4->sin_addr.S_un.S_un_b.s_b3;
				*a4											= sockaddr_ipv4->sin_addr.S_un.S_un_b.s_b4;
				//printf("\tIPv4 address %s\n", inet_ntoa(sockaddr_ipv4->sin_addr) );
				addressFound								= true;
			}
			break;
		case AF_INET6	:
			info_printf("AF_INET6 (IPv6)");
			// the InetNtop function is available on Windows Vista and later
			sockaddr_ipv6								= (struct ::sockaddr_in6 *) ptr->ai_addr;
			//info_printf("IPv6 address %s", InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipwstringbuffer, 46) );

#if defined(__WINDOWS__)
			sockaddr_ip									= (LPSOCKADDR)ptr->ai_addr;
			ipbufferlength								= 46;	// The buffer length is changed by each call to WSAAddresstoString, so we need to set it for each iteration through the loop for safety
			iRetval										= ::WSAAddressToStringW(sockaddr_ip, (DWORD) ptr->ai_addrlen, NULL, ipwstringbuffer, &ipbufferlength );	// We use WSAAddressToString since it is supported on Windows XP and later
			if (iRetval) {
				warning_printf("WSAAddressToString failed with code 0x%X: '%s'.", ::WSAGetLastError(), ::nwol::getWindowsErrorAsString(::WSAGetLastError()).c_str() );
			}
			else
				::wprintf(L"IPv6 address '%s'.\n", ipwstringbuffer);
#endif
			break;
		}

		info_printf("Socket type: ");
		switch (ptr->ai_socktype) {
		case 0				: info_printf("Unspecified"								);	break;
		case SOCK_STREAM	: info_printf("SOCK_STREAM (stream)"					);	break;
		case SOCK_DGRAM		: info_printf("SOCK_DGRAM (datagram)"					);	break;
		case SOCK_RAW		: info_printf("SOCK_RAW (raw)"							);	break;
		case SOCK_RDM		: info_printf("SOCK_RDM (reliable message datagram)"	);	break;
		case SOCK_SEQPACKET	: info_printf("SOCK_SEQPACKET (pseudo-stream packet)"	);	break;
		default:
			info_printf("Other %ld", ptr->ai_socktype);
			break;
		}
		info_printf("Protocol: ");
		switch (ptr->ai_protocol) {
		case 0				: info_printf("Unspecified"								);	break;
		case IPPROTO_TCP	: info_printf("IPPROTO_TCP (TCP)"						);	break;
		case IPPROTO_UDP	: info_printf("IPPROTO_UDP (UDP)"						);	break;
		default:
			info_printf("Other %ld", ptr->ai_protocol);
			break;
		}
		++iAddress;
		info_printf("Length of this sockaddr: %u", (uint32_t)ptr->ai_addrlen);
		info_printf("Canonical name: %s", ptr->ai_canonname);
		if(addressFound)
			break;
	}
	::freeaddrinfo((::addrinfo*)createdAddrInfo);
//#else
//	struct hostent									* hp		= gethostbyname(host_name);
//
//	ree_if(hp == NULL, "Could not get host by name.");
//
//	b1											= hp->h_addr_list[0][0];
//	b2											= hp->h_addr_list[0][1];
//	b3											= hp->h_addr_list[0][2];
//	b4											= hp->h_addr_list[0][3];
//#endif

	return iAddress;
}
