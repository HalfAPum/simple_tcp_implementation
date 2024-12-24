//
// Created by o.narvatov on 12/24/2024.
//

#include "SocketFactoryMock.h"

SOCKET SocketFactoryMock::createUDPSocket(int type, bool bindPort, uint16_t port) {
    return 0;
}

uint16_t SocketFactoryMock::getSocketPort(SOCKET socket) {
    return 0;
}