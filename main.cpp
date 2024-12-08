#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

#include "Constants.h"
#include "SimpleTCP.h"

int main() {
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