//
// Created by o.narvatov on 12/24/2024.
//

#ifndef SOCKETFACTORYMOCK_H
#define SOCKETFACTORYMOCK_H
#include "socket/base/SocketFactory.h"


class SocketFactoryMock final : public SocketFactory {
public:
    SOCKET createUDPSocket(int type, bool bindPort, uint16_t port) override;
    uint16_t getSocketPort(SOCKET socket) override;
};



#endif //SOCKETFACTORYMOCK_H
