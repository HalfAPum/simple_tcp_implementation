//
// Created by o.narvatov on 12/8/2024.
//

#include "LocalConnection.h"
#include "../Constants.h"

#include <iostream>

#include <ws2tcpip.h>

#include "socket/base/SocketFactory.h"


SOCKET LocalConnection::createLocalSocket(const bool randomPort) {
    const SOCKET connectionSocket = SocketFactory::singleton->createUDPSocket(
        SOCK_DGRAM,
        true,
        randomPort ? EPHEMERAL_PORT : localPort
    );

    localPort = SocketFactory::singleton->getSocketPort(connectionSocket);

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
