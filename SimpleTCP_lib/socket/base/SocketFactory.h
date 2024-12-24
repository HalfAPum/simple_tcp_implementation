//
// Created by o.narvatov on 12/8/2024.
//

#ifndef SOCKETFACTORY_H
#define SOCKETFACTORY_H
#include <cstdint>
#include <winsock2.h>


struct SocketFactory {
    virtual ~SocketFactory() = default;

    SOCKET createUDPSocket(int type, bool bindPort);
    virtual SOCKET createUDPSocket(int type, bool bindPort, uint16_t port) { return 0;}
    virtual uint16_t getSocketPort(SOCKET socket) { return 0; }

    static SocketFactory* singleton;

    static SocketFactory *initialize(SocketFactory* other);
};



#endif //SOCKETFACTORY_H
