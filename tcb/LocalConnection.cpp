//
// Created by o.narvatov on 12/8/2024.
//

#include "LocalConnection.h"
#include "../Constants.h"

#include <iostream>


SOCKET LocalConnection::createLocalSocket(const bool randomPort) {
    const SOCKET connectionSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (connectionSocket == INVALID_SOCKET) {
        std::cout << "socket creation faield with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        throw std::exception();
    }

    sockaddr_in sockstr {};
    sockstr.sin_family = AF_INET;
    inet_pton(AF_INET, ADDR_TO_BIND, &sockstr.sin_addr.s_addr);
    //Let winsock define port for us or put localPort
    sockstr.sin_port = htons((randomPort) ? 0 : localPort);
    constexpr int sockstrLen = sizeof(sockstr);

    if (bind(connectionSocket, (SOCKADDR *) & sockstr, sockstrLen)) {
        printf("bind failed with error %d\n", WSAGetLastError());
        throw std::exception();
    }

    sockaddr_in sa {};
    int saLen = sizeof(sa);
    if (getsockname(connectionSocket, (SOCKADDR *) & sa, &saLen)) {
        printf("getsockname failed with error %d\n", WSAGetLastError());
        throw std::exception();
    }

    localPort = ntohs(sa.sin_port);
    std::cout << "Binded local port: " << (int)localPort << std::endl;

    return connectionSocket;
}


void LocalConnection::createForeignSocketAddress(const uint32_t &_foreignAddress, const uint16_t &_foreignPort) {
    foreignAddress = _foreignAddress;
    foreignPort = _foreignPort;

    auto *sockstr = new sockaddr_in();
    sockstr->sin_family = AF_INET;
    inet_pton(AF_INET, ADDR_TO_BIND, &sockstr->sin_addr.s_addr);
    sockstr->sin_port = htons(foreignPort);

    foreignSockaddrr = reinterpret_cast<sockaddr *>(sockstr);
}
