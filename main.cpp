#include <cstdint>
#include <iomanip>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "IPv4Header.h"

#define ADDR_TO_BIND "127.0.0.1"
#define DEFAULT_PORT 8080
#define BUFFLEN 65535
#define RECV_ERROR (-1)

constexpr int IP_HEADER_LENGTH = 20;
constexpr int TCP_HEADER_LENGTH = 20;
constexpr int TCP_SEGMENT_LENGTH = IP_HEADER_LENGTH + TCP_HEADER_LENGTH;

bool checkResultFail(const bool result, const std::string &actionName, const SOCKET socket) {
    if (!result) return false;

    std::cout << actionName << " failed with error: " << WSAGetLastError() << std::endl;

    closesocket(socket);

    return true;
}

bool validate(const bool result, const SOCKET socket, const std::string &message) {
    if (!result) return false;

    std::cout << message << std::endl;

    closesocket(socket);

    return true;
}

void printMessage(char buffer[BUFFLEN], const int size) {
    std::cout << "Message size: " << size << std::endl;

    for(int i = 0 ; i < size ; ++i){
        std::cout << buffer[i];
    }
    std::cout << std::endl;
    printf("\n");
}

int main() {
    WSAData wsaData = {};

    //Initialize winsock
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup faield with error: " << iResult << std::endl;
        return 1;
    }

    //Try IPPROTO_RAW when succeed with UDP
    SOCKET listenSocket = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
    if (listenSocket == INVALID_SOCKET) {
        std::cout << "socket creation faield with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    if (!setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEADDR, nullptr, 0)) {
        perror("setsockopt");
        return 1;
    }

    sockaddr_in sockstr {};
    sockstr.sin_addr.s_addr = inet_addr(ADDR_TO_BIND);
    sockstr.sin_family = AF_INET;
    sockstr.sin_port = DEFAULT_PORT;
    constexpr auto sockstr_size = static_cast<socklen_t>(sizeof(sockstr));

    iResult = bind(listenSocket, reinterpret_cast<sockaddr *>(&sockstr), sockstr_size);
    if (checkResultFail(iResult == SOCKET_ERROR, "bind", listenSocket)) {
        return 1;
    }
    std::cout << "bind result " << iResult << std::endl;

    char recvbuf[BUFFLEN];
    int recvbuflen = BUFFLEN;

    while (true) {
        const int recvResult = recv(listenSocket, recvbuf, recvbuflen, 0);

        if (checkResultFail(recvResult == RECV_ERROR, "recvResult", listenSocket)) {
            return 1;
        }

        //Verify received Result size.
        //We should receive IP and TCP headers (each is 20 bytes long).
        //Data payload is optional.
        if (validate(recvResult < TCP_SEGMENT_LENGTH, listenSocket,
            "Received message has size: " + std::to_string(recvResult) +
            ". This is less than TCP_SEGMENT_LENGTH " + std::to_string(TCP_SEGMENT_LENGTH)
        )) {
            return 1;
        }

        auto ipv4Header = IPv4Header::parseIPv4Header(recvbuf);

        printMessage(recvbuf, recvResult);
        closesocket(listenSocket);
        break;
    }

    return 0;
}

// TCP Header:
//
// Source Port (16 bits)
// Destination Port (16 bits)
// Sequence Number (32 bits)
// Acknowledgment Number (32 bits)
// Data Offset (4 bits)
// Reserved (6 bits)
// Flags (6 bits)  
// Window Size (16 bits)
// Checksum (16 bits)
// Urgent Pointer (16 bits)
// Options (variable length)
// Data (variable length)