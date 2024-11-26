//
// Created by o.narvatov on 11/24/2024.
//

#ifndef LOCALCONNECTION_H
#define LOCALCONNECTION_H

#include <cstdint>
#include <iostream>
#include <string>

struct LocalConnection {
    const uint32_t bindAddress;
    const uint16_t localPort;
    uint32_t foreignAddress = 0;
    uint16_t foreignPort = 0;
    //<local socket, foreign socket>

    LocalConnection(
        const uint32_t _bindAddress,
        const uint16_t _localPort
    ) : bindAddress(_bindAddress),
        localPort(_localPort)
        {}

    [[nodiscard]] std::string getTCBKey() const { return std::to_string(localPort); }
};


#endif //LOCALCONNECTION_H
