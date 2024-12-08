//
// Created by o.narvatov on 12/8/2024.
//

#include "SocketFactory.h"

#include <cstdio>
#include <exception>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <psdk_inc/_ip_types.h>


SOCKET SocketFactory::createUDPSocket(const int type, const bool bindPort, const uint16_t port) {
    const SOCKET connectionSocket = socket(AF_INET, type, IPPROTO_UDP);
    if (connectionSocket == INVALID_SOCKET) {
        std::cout << "socket creation faield with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        throw std::exception();
    }

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ADDR_TO_BIND, &addr.sin_addr.s_addr);
    //Let winsock define port for us if port==0 or put specified port
    addr.sin_port = htons(port);
    constexpr int addrLen = sizeof(addr);

    if (bindPort) {
        if (bind(connectionSocket, reinterpret_cast<SOCKADDR *>(&addr), addrLen)) {
            closesocket(connectionSocket);
            printf("bind failed with error %d\n", WSAGetLastError());
            throw std::exception();
        }
    }

    return connectionSocket;
}

uint16_t SocketFactory::getSocketPort(const SOCKET socket) {
    sockaddr_in addr {};
    int saLen = sizeof(addr);
    if (getsockname(socket, reinterpret_cast<SOCKADDR *>(&addr), &saLen)) {
        printf("getsockname failed with error %d\n", WSAGetLastError());
        throw std::exception();
    }

    return ntohs(addr.sin_port);
}

