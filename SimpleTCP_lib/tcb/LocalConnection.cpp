//
// Created by o.narvatov on 12/8/2024.
//

#include "LocalConnection.h"
#include "../Constants.h"

#include <iostream>

#include "../socket/SocketFactory.h"
#include <ws2tcpip.h>


SOCKET LocalConnection::createLocalSocket(const bool randomPort) {
    const SOCKET connectionSocket = SocketFactory::createUDPSocket(
        SOCK_DGRAM,
        true,
        randomPort ? EPHEMERAL_PORT : localPort
    );

    localPort = SocketFactory::getSocketPort(connectionSocket);
    std::cout << "Binded local port: " << static_cast<int>(localPort) << std::endl;

    return connectionSocket;
}


void LocalConnection::createForeignSocketAddress(const uint16_t &_foreignPort) {
    foreignPort = _foreignPort;

    auto *sockstr = new sockaddr_in();
    sockstr->sin_family = AF_INET;
    inet_pton(AF_INET, ADDR_TO_BIND, &sockstr->sin_addr.s_addr);
    sockstr->sin_port = htons(foreignPort);

    foreignSockaddrr = reinterpret_cast<sockaddr *>(sockstr);
}