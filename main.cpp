
#include "Constants.h"
#include "SimpleTCP.h"

int main() {
    const auto listenPort = std::stoi(getenv(LISTEN_PORT));
    const auto passiveConnection = std::stoi(getenv(PASSIVE_CONNECTION)) != 0;

    int connectionForeignPort = EPHEMERAL_PORT;

    if (!passiveConnection) {
        connectionForeignPort = std::stoi(getenv(CONNECTION_FOREIGN_PORT));
    }

    SimpleTCP simpleTcp (ADDR_TO_BIND, listenPort);

    if (!simpleTcp.initialize()) return -1;

    simpleTcp.open(EPHEMERAL_PORT, connectionForeignPort, passiveConnection);

    // while (true);
}