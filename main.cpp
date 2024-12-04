
#include "SimpleTCP.h"

#define LISTEN_PORT "LISTEN_PORT"
#define PASSIVE_CONNECTION "PASSIVE_CONNECTION"
#define CONNECT_PORT "CONNECT_PORT"

int main() {
    const auto listenPort = std::stoi(getenv(LISTEN_PORT));
    const auto passiveConnection = std::stoi(getenv(PASSIVE_CONNECTION)) != 0;

    int connectPort = 0;

    if (!passiveConnection) {
        connectPort = std::stoi(getenv(CONNECT_PORT));
    }

    SimpleTCP simpleTcp;

    simpleTcp.initialize();
    simpleTcp.open(listenPort, passiveConnection, connectPort);

    while (true);
}