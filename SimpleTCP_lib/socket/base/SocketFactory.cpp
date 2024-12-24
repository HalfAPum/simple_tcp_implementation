//
// Created by o.narvatov on 12/8/2024.
//

#include "SocketFactory.h"

#include "Constants.h"

SOCKET SocketFactory::createUDPSocket(const int type, const bool bindPort) {
    return createUDPSocket(type, bindPort, EPHEMERAL_PORT);
}

SocketFactory* SocketFactory::singleton = nullptr;

SocketFactory *SocketFactory::initialize(SocketFactory *other) {
    singleton = other;

    return singleton;
}