#include "Network.h"


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


	localIP.reserve(IP_SIZE);

	status = initilize(port, protocol);
	if (status != NET_OK)
		return status;

	// if server does not contain a dotted quad IP address nnn.nnn.nnn.nnn
	if ((remoteAddr.sin_addr.s_addr = inet_addr(server.c_str())) == INADDR_NONE) {
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
		server = inet_ntoa(remoteAddr.sin_addr);
	}

	// set local IP address
	getLocalIp(localIP);
	localAddr.sin_addr.s_addr = inet_addr(localIP.c_str());
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
	localIP = inet_ntoa(in_addr);

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

}

INT32 Network::ReadData(CHAR* data, INT32& size, LPSTR senderIP, USHORT& port)
{

}

INT32 Network::SendData(const CHAR* data, INT32& size, const CHAR* remoteIP)
{

}

INT32 Network::SendData(const CHAR* data, INT32& size, const CHAR* remoteIP, USHORT port)
{

}