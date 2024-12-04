//
// Created by o.narvatov on 11/24/2024.
//

#ifndef LOCALCONNECTION_H
#define LOCALCONNECTION_H

#include <cstdint>
#include <iostream>
#include <string>
#include <ws2tcpip.h>
#include <psdk_inc/_ip_types.h>

struct LocalConnection {
    //Local socket info
    const uint32_t bindAddress;
    const uint16_t localPort;
    //Foreign socket info
    uint32_t foreignAddress = 0;
    uint16_t foreignPort = 0;
    sockaddr* foreignSockaddrr;
    socklen_t foreignSockaddrrLength;


    LocalConnection(
        const uint32_t _bindAddress,
        const uint16_t _localPort
    ) : bindAddress(_bindAddress),
        localPort(_localPort),
        foreignSockaddrr(nullptr),
        foreignSockaddrrLength(0)
        {}

    [[nodiscard]] std::string getTCBKey() const { return std::to_string(localPort); }

    void createForeignSocket(const uint32_t &_foreignAddress, const uint16_t &_foreignPort) {
        foreignAddress = _foreignAddress;
        foreignPort = _foreignPort;

        auto *sockstr = new sockaddr_in();
        sockstr->sin_addr.s_addr = htonl(foreignAddress);
        sockstr->sin_family = AF_INET;
        sockstr->sin_port = htons(8080);

        foreignSockaddrr = reinterpret_cast<sockaddr *>(sockstr);
        foreignSockaddrrLength = static_cast<socklen_t>(sizeof(*sockstr));
    }

    ~LocalConnection() {
        delete foreignSockaddrr;
    }

};


#endif //LOCALCONNECTION_H
