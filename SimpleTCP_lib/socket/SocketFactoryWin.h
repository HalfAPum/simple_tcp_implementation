//
// Created by o.narvatov on 12/24/2024.
//

#ifndef SOCKETFACTORYWIN_H
#define SOCKETFACTORYWIN_H
#include "base/SocketFactory.h"


class SocketFactoryWin final : public SocketFactory {
public:
    SOCKET createUDPSocket(int type, bool bindPort, uint16_t port) override;
    uint16_t getSocketPort(SOCKET socket) override;
};



#endif //SOCKETFACTORYWIN_H
