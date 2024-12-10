//
// Created by o.narvatov on 12/8/2024.
//

#ifndef SOCKETFACTORY_H
#define SOCKETFACTORY_H
#include <cstdint>
#include <winsock2.h>

#include "../Constants.h"


struct SocketFactory {
    static SOCKET createUDPSocket(int type, bool bindPort, uint16_t port = EPHEMERAL_PORT);
    static uint16_t getSocketPort(SOCKET socket);
};



#endif //SOCKETFACTORY_H
