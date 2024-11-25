//
// Created by o.narvatov on 11/24/2024.
//

#ifndef LOCALCONNECTION_H
#define LOCALCONNECTION_H

#include <string>

struct LocalConnection {
    const unsigned long bindAddress;
    const unsigned localPort;
    //<local socket, foreign socket>

    LocalConnection(
        const unsigned long _bindAddress,
        const unsigned _localPort
    ) : bindAddress(_bindAddress),
        localPort(_localPort)
        {}

    [[nodiscard]] std::string getTCBKey() const { return std::to_string(localPort); }
};


#endif //LOCALCONNECTION_H
