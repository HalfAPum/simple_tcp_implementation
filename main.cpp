//
// Created by o.narvatov on 12/10/2024.
//

#include <iostream>

#include "Constants.h"
#include "SimpleTCP.h"

int main() {
    std::cout << "FUCK ME AND YOU BITCH" << std::endl;
    const auto listenPort = std::stoi(getenv(LISTEN_PORT));
    const auto passiveConnection = std::stoi(getenv(PASSIVE_CONNECTION)) != 0;

    int connectionForeignPort = EPHEMERAL_PORT;

    if (!passiveConnection) {
        connectionForeignPort = std::stoi(getenv(CONNECTION_FOREIGN_PORT));
    }

    SimpleTCP simpleTcp {};

    if (!simpleTcp.initialize()) return -1;

    simpleTcp.open(listenPort, connectionForeignPort, passiveConnection);
    while (true) {}
}
