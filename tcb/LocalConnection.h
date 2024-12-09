//
// Created by o.narvatov on 11/24/2024.
//

#ifndef LOCALCONNECTION_H
#define LOCALCONNECTION_H

#include <cstdint>
#include <winsock2.h>

struct LocalConnection {
    //Local socket info
    const uint32_t bindAddress;
    uint16_t localPort;
    //Foreign socket info
    uint16_t foreignPort = 0;
    sockaddr* foreignSockaddrr;


    LocalConnection(
        const uint32_t _bindAddress,
        const uint16_t _localPort
    ) : bindAddress(_bindAddress),
        localPort(_localPort),
        foreignSockaddrr(nullptr)
        {}

    ~LocalConnection() {
        delete foreignSockaddrr;
    }

    SOCKET createLocalSocket(bool randomPort);
    void createForeignSocketAddress(const uint16_t &_foreignPort);
};


#endif //LOCALCONNECTION_H
