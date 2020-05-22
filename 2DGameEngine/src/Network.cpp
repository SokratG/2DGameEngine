#include "Network.h"

#define MAX_CONN 1
#define PTON_OK 1
using namespace NetParam;

Network::Network()
{
	BufferLength = BUFFER_LENGTH;
	sock = NULL;
	ret = 0;
	mode = MODE::UNINITIALIZED;
	type = CONNECT_TYPE::UNCONNECTED;
	bound = FALSE;
	remoteAddrSize = 0;
	Initilized = FALSE;
	ZeroMemory(&wsaData, sizeof(WSADATA));
}

Network::~Network()
{
	closeSocket();
}

INT32 Network::initilize(INT32 port, CONNECT_TYPE protocol)
{
	INT32 status = 0;
	if (Initilized)
		closeSocket();

	mode = MODE::UNINITIALIZED;
	status = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (status != 0)
		return ((status << 16) + NET_INIT_FAILED);
	
	switch (protocol)
	{
	case NetParam::UDP:
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock == INVALID_SOCKET) {
			WSACleanup();
			status = WSAGetLastError();
			return ((status << 16) + NET_INVALID_SOCKET);
		}
		type = UDP;
		break;
	case NetParam::TCP:
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == INVALID_SOCKET) {
			WSACleanup();
			status = WSAGetLastError();
			return ((status << 16) + NET_INVALID_SOCKET);
		}
		type = UNCONNECTED_TCP;
		break;
	default:
		return (NET_INIT_FAILED);
	}

	// non blocking socket mode
	ULONG larg;
	INT32 res = ioctlsocket(sock, FIONBIO, &larg);
	if (res == SOCKET_ERROR) {
		WSACleanup();
		status = WSAGetLastError();
		return ((status << 16) + NET_INVALID_SOCKET);
	}

	// set local family and port
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = htons((USHORT)port);
	// set remote family and port
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons((USHORT)port);

	Initilized = TRUE;
	return NET_OK;
}

INT32 Network::createClient(string server, INT32 port, CONNECT_TYPE protocol)
{
	INT32 status = 0;
	string localIP;
	ADDRINFOA host;
	ADDRINFOA* pResult = nullptr;
	CHAR addr[16];
	ZeroMemory(addr, sizeof(addr));

	localIP.reserve(IP_SIZE);

	status = initilize(port, protocol);
	if (status != NET_OK)
		return status;

	// if server does not contain a dotted quad IP address nnn.nnn.nnn.nnn
	if (inet_pton(AF_INET, server.c_str(), &remoteAddr.sin_addr.s_addr) == PTON_OK) {
		ZeroMemory(&host, sizeof(host));
		host.ai_family = AF_INET;
		host.ai_socktype = SOCK_STREAM;
		host.ai_protocol = IPPROTO_TCP;
		// get address information
		status = getaddrinfo(server.c_str(), nullptr, &host, &pResult);
		if (status != 0) {
			status = WSAGetLastError();
			return ((status << 16) + NET_DOMAIN_NOT_FOUND);
		}
		// get IP address of server
		remoteAddr.sin_addr = (reinterpret_cast<SOCKADDR_IN*>(pResult->ai_addr))->sin_addr;
		inet_ntop(AF_INET, &remoteAddr.sin_addr, addr, sizeof(addr));
		server = addr;

	}

	// set local IP address
	getLocalIp(localIP);
	inet_pton(AF_INET, localIP.c_str(), &localAddr.sin_addr.s_addr);
	mode = CLIENT;

	return NET_OK;
}


INT32 Network::createServer(INT32 port, NetParam::CONNECT_TYPE protocol)
{
	INT32 status = 0;
	status = initilize(port, protocol);
	if (status != NET_OK)
		return status;

	// listen on all addresses
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, (SOCKADDR*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR) {
		status = WSAGetLastError();
		return ((status << 16) + NET_BIND_FAILED);
	}
	
	bound = TRUE;
	mode = SERVER;

	return NET_OK;

}


string Network::getErrorMessage(INT32 error)
{
	INT32 socketError = error >> 16;
	string MessageError;

	error &= STATUS_MASK;
	// check unknow error code
	if (error > ERROR_CODES - 2)
		error = ERROR_CODES - 1;

	MessageError = codes[error];
	for (size_t i = 0; i < SOCK_CODES; ++i) {
		if (errorCodes[i].sockErr == socketError) {
			MessageError += errorCodes[i].message;
			break;
		}
	}
	return MessageError;
}

INT32 Network::getLocalIp(string& localIP) const
{
	CHAR hostname[64];
	ADDRINFOA host;
	ADDRINFOA* pResult = nullptr;
	INT32 status = 0;
	CHAR addr[16];
	ZeroMemory(addr, sizeof(addr));

	gethostname(hostname, 64);
	ZeroMemory(&host, sizeof(ADDRINFOA));
	host.ai_family = AF_INET;
	host.ai_socktype = SOCK_STREAM;
	host.ai_protocol = IPPROTO_TCP;

	status = getaddrinfo(hostname, NULL, &host, &pResult);
	if (status != 0) {                // if getaddrinfo failed
		status = WSAGetLastError();        
		return ((status << 16) + NET_ERROR);
	}

	IN_ADDR in_addr = ((SOCKADDR_IN*)pResult->ai_addr)->sin_addr;
	inet_ntop(AF_INET, &remoteAddr.sin_addr, addr, sizeof(addr));
	localIP = addr;

	return NET_OK;
}

INT32 Network::closeSocket()
{
	INT32 status_code;

	type = CONNECT_TYPE::UNCONNECTED;
	bound = FALSE;
	Initilized = FALSE;

	if (closesocket(sock) == SOCKET_ERROR) {
		status_code = WSAGetLastError();
		// if WOULDBLOCK don't report error
		if (status_code != WSAEWOULDBLOCK)
			return ((status_code << 16) + NET_ERROR);
	}
	if (WSACleanup())
		return NET_ERROR;

	return NET_OK;
}

//
INT32 Network::ReadData(CHAR* data, INT32& size, LPSTR senderIP)
{
	INT32 status = 0;
	INT32 readSize = size;
	size = 0;
	CHAR addr[16];
	ZeroMemory(addr, sizeof(addr));
	// no recieve unbound socket
	if (bound == FALSE)
		return NET_OK;

	if (mode == SERVER && type == CONNECTED_TCP) {
		ret = listen(sock, MAX_CONN);
		if (ret == SOCKET_ERROR) {
			status = WSAGetLastError();
			return ((status << 16) + NET_ERROR);
		}
		SOCKET isock = accept(sock, nullptr, nullptr);
		if (isock == INVALID_SOCKET) {
			status = WSAGetLastError();
			if(status != WSAEWOULDBLOCK)	// skip WSAEWOULDBLOCK error
				return ((status << 16) + NET_ERROR);
			return NET_OK;	// no connection
		}
		closesocket(sock);
		sock = isock;
		type = CONNECTED_TCP;
	}

	if (mode == CLIENT && type == UNCONNECTED_TCP)
		return NET_OK; // no connection

	if (sock != NULL) {
		remoteAddrSize = sizeof(remoteAddr);
		ret = recvfrom(sock, data, readSize, 0, (SOCKADDR*)&remoteAddr, &remoteAddrSize);
		if (ret == SOCKET_ERROR) {
			status = WSAGetLastError();
			if (status != WSAEWOULDBLOCK)
				return ((status << 16) + NET_ERROR);
			ret = 0; // clear SOCKET_ERROR
		}
		else if (ret == 0 && type == CONNECTED_TCP) // check TCP connection did close
			return ((REMOTE_DISCONNECT << 16) + NET_ERROR); // return Remote Disconnect error
		// copy data to buffer
		if (ret) {
			inet_ntop(AF_INET, &remoteAddr.sin_addr, addr, sizeof(addr));
			strncpy_s(senderIP, IP_SIZE, addr, IP_SIZE);
		}
			
		size = ret;
	}
	return NET_OK;

}

INT32 Network::ReadData(CHAR* data, INT32& size, LPSTR senderIP, USHORT& port)
{
	INT32 status = 0;
	INT32 readSize = size;
	size = 0;
	CHAR addr[16];
	ZeroMemory(addr, sizeof(addr));
	// no recieve unbound socket
	if (bound == FALSE)
		return NET_OK;

	if (mode == SERVER && type == CONNECTED_TCP) {
		ret = listen(sock, MAX_CONN);
		if (ret == SOCKET_ERROR) {
			status = WSAGetLastError();
			return ((status << 16) + NET_ERROR);
		}
		SOCKET isock = accept(sock, nullptr, nullptr);
		if (isock == INVALID_SOCKET) {
			status = WSAGetLastError();
			if (status != WSAEWOULDBLOCK)	// skip WSAEWOULDBLOCK error
				return ((status << 16) + NET_ERROR);
			return NET_OK;	// no connection
		}
		closesocket(sock);
		sock = isock;
		type = CONNECTED_TCP;
	}

	if (mode == CLIENT && type == UNCONNECTED_TCP)
		return NET_OK; // no connection


	if (sock != NULL) {
		remoteAddrSize = sizeof(remoteAddr);
		ret = recvfrom(sock, data, readSize, 0, (SOCKADDR*)&remoteAddr, &remoteAddrSize);
		if (ret == SOCKET_ERROR) {
			status = WSAGetLastError();
			if (status != WSAEWOULDBLOCK)
				return ((status << 16) + NET_ERROR);
			ret = 0; // clear SOCKET_ERROR
		}
		else if (ret == 0 && type == CONNECTED_TCP) // check TCP connection did close
			return ((REMOTE_DISCONNECT << 16) + NET_ERROR); // return Remote Disconnect error
		// copy data to buffer and setup port
		if (ret) {
			inet_ntop(AF_INET, &remoteAddr.sin_addr, addr, sizeof(addr));
			strncpy_s(senderIP, IP_SIZE, addr, IP_SIZE);
			port = remoteAddr.sin_port;
		}
		size = ret;
	}
	return NET_OK;

}

INT32 Network::SendData(const CHAR* data, INT32& size, LPCSTR remoteIP)
{
	INT32 status = 0;
	INT32 sendSize = size;
	size = 0;

	if (mode == SERVER)
		inet_pton(AF_INET, remoteIP, &remoteAddr.sin_addr.s_addr);

	if (mode == CLIENT && type == UNCONNECTED_TCP) {
		ret = connect(sock, (SOCKADDR*)&remoteAddr, sizeof(remoteAddr));
		if (ret == SOCKET_ERROR) {
			status = WSAGetLastError();
			if (status == WSAEISCONN) { // check if connected 
				ret = 0;
				type = CONNECTED_TCP;
			}
			else {
				if (status == WSAEWOULDBLOCK || status == WSAEALREADY)
					return NET_OK;	// no connection
				else
					return ((status << 16) + NET_ERROR);
			}
		}
	}

	ret = sendto(sock, data, sendSize, 0, (SOCKADDR*)&remoteAddr, sizeof(remoteAddr));
	if (ret == SOCKET_ERROR) {
		status = WSAGetLastError();
		return ((status << 16) + NET_ERROR);
	}
	bound = TRUE;	// binding by sendto if unbound
	size = ret;
	return NET_OK;
}

INT32 Network::SendData(const CHAR* data, INT32& size, LPCSTR remoteIP, const USHORT port)
{
	INT32 status = 0;
	INT32 sendSize = size;
	size = 0;

	if (mode == SERVER)
	{
		inet_pton(AF_INET, remoteIP, &remoteAddr.sin_addr.s_addr);
		remoteAddr.sin_port = port;
	}


	if (mode == CLIENT && type == UNCONNECTED_TCP) {
		ret = connect(sock, (SOCKADDR*)&remoteAddr, sizeof(remoteAddr));
		if (ret == SOCKET_ERROR) {
			status = WSAGetLastError();
			if (status == WSAEISCONN) { // check if connected 
				ret = 0;
				type = CONNECTED_TCP;
			}
			else {
				if (status == WSAEWOULDBLOCK || status == WSAEALREADY)
					return NET_OK;	// no connection
				else
					return ((status << 16) + NET_ERROR);
			}
		}
	}

	ret = sendto(sock, data, sendSize, 0, (SOCKADDR*)&remoteAddr, sizeof(remoteAddr));
	if (ret == SOCKET_ERROR) {
		status = WSAGetLastError();
		return ((status << 16) + NET_ERROR);
	}
	bound = TRUE;
	size = ret;
	return NET_OK;
}