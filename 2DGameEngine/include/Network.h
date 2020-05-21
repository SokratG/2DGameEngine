#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <memory.h>
#include <stdio.h>
#include <string>
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using std::string;

namespace NetParam
{
    const USHORT DEFAULT_PORT = 48252;
    const INT32 MIN_PORT = 1024;
    const INT32 BUFFER_LENGTH = 4096;
    const INT32 IP_SIZE = 16;     // size of "nnn.nnn.nnn.nnn"

    enum MODE {
        UNINITIALIZED = 0,
        SERVER = 1,
        CLIENT = 2
    };
    

    // Connection type
    enum CONNECT_TYPE {
        UNCONNECTED = -1,
        UDP = 0,
        TCP = 1,
        UNCONNECTED_TCP = 2,
        CONNECTED_TCP = 3
    };
    

    // Status codes
    const INT32 STATUS_MASK = 0X0FFFF;  
    const INT32 NET_OK = 0;
    const INT32 NET_ERROR = 1;
    const INT32 NET_INIT_FAILED = 2;
    const INT32 NET_INVALID_SOCKET = 3;
    const INT32 NET_GET_HOST_BY_NAME_FAILED = 4;
    const INT32 NET_BIND_FAILED = 5;
    const INT32 NET_CONNECT_FAILED = 6;
    const INT32 NET_ADDR_IN_USE = 7;
    const INT32 NET_DOMAIN_NOT_FOUND = 8;

    const INT32 PACKETS_PER_SEC = 30;     // Number of packets to send per second
    const FLOAT NET_TIME = 1.0f / PACKETS_PER_SEC;    // time between net transmissions
    const INT32 MAX_ERRORS = PACKETS_PER_SEC * 30;  // Packets/Sec * 30 Sec
    const INT32 MAX_COMM_WARNINGS = 10;       // max packets out of sync before time reset

    // Connection response messages, ===== MUST BE SAME SIZE =====
    const INT32 RESPONSE_SIZE = 12;
    const CHAR CLIENT_ID[RESPONSE_SIZE] = "Client OK";  // client ID
    const CHAR SERVER_ID[RESPONSE_SIZE] = "Server OK";  // server ID
    const CHAR SERVER_FULL[RESPONSE_SIZE] = "Server Full";  // server full

    const INT32 ERROR_CODES = 10;
    // Network error codes
    static const CHAR* codes[ERROR_CODES] = {
        "No errors reported",
        "General network error: ",
        "Network init failed: ",
        "Invalid socket: ",
        "Get host by name failed: ",
        "Bind failed: ",
        "Connect failed: ",
        "Port already in use: ",
        "Domain not found: ",
        "Unknown network error: "
    };

    struct ErrorCode
    {
        INT32 sockErr;        // Windows Socket Error Code
        CHAR message[64];
    };

    const INT32 REMOTE_DISCONNECT = 0x2775;

    const INT32 SOCK_CODES = 29;
    // Windows Socket Error Codes
    static const ErrorCode errorCodes[SOCK_CODES] = {
        {0x2714, "A blocking operation was interrupted"},
        {0x271D, "Socket access permission violation"},
        {0x2726, "Invalid argument"},
        {0x2728, "Too many open sockets"},
        {0x2735, "Operation in progress"},
        {0x2736, "Operation on non-socket"},
        {0x2737, "Address missing"},
        {0x2738, "Message bigger than buffer"},
        {0x273F, "Address incompatible with protocol"},
        {0x2740, "Address is already in use"},
        {0x2741, "Address not valid in current context"},
        {0x2742, "Network is down"},
        {0x2743, "Network unreachable"},
        {0x2744, "Connection broken during operation"},
        {0x2745, "Connection aborted by host software"},
        {0x2746, "Connection reset by remote host"},
        {0x2747, "Insufficient buffer space"},
        {0x2748, "Connect request on already connected socket"},
        {0x2749, "Socket not connected or address not specified"},
        {0x274A, "Socket already shut down"},
        {0x274C, "Operation timed out"},
        {0x274D, "Connection refused by target"},
        {0x274E, "Cannot translate name"},
        {0x274F, "Name too long"},
        {0x2750, "Destination host down"},
        {0x2751, "Host unreachable"},
        {0x276B, "Network cannot initialize, system unavailable"},
        {0x276D, "Network has not been initialized"},
        {0x2775, "Remote has disconnected"},
    };

}


class Network
{
   
private:

    WSADATA wsaData;
    SOCKET sock;
    INT32 ret;
    INT32 remoteAddrSize;
    NetParam::MODE mode;
    BOOL bound;
    NetParam::CONNECT_TYPE type;
    UINT BufferLength;
    BOOL Initilized;
    SOCKADDR_IN remoteAddr, localAddr;

    INT32 initilize(INT32 port, NetParam::CONNECT_TYPE protocol);
public:
    Network();
    virtual ~Network();
    INT32 createClient(string server, INT32 port, NetParam::CONNECT_TYPE protocol);
    INT32 createServer(INT32 port, NetParam::CONNECT_TYPE protocol);

    // getters
    string getErrorMessage(INT32 error);
    NetParam::MODE getMode() { return mode; }
    INT32 getLocalIp(string& localIP) const;
    INT32 closeSocket();

    //
    INT32 ReadData(CHAR* data, INT32& size, LPSTR senderIP);
    INT32 ReadData(CHAR* data, INT32& size, LPSTR senderIP, USHORT& port);
    INT32 SendData(const CHAR* data, INT32& size, const CHAR* remoteIP);
    INT32 SendData(const CHAR* data, INT32& size, const CHAR* remoteIP, USHORT port);
};

