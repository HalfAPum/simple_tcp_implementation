
#include "SimpleTCP.h"

#define LISTEN_PORT "LISTEN_PORT"
#define PASSIVE_CONNECTION "PASSIVE_CONNECTION"
#define CONNECT_PORT "CONNECT_PORT"

int main() {
    std::cout << "Env variables" << std::endl;
    const auto listenPort = std::stoi(getenv(LISTEN_PORT));
    std::cout << std::to_string(listenPort) << std::endl;
    const auto passiveConnection = std::stoi(getenv(PASSIVE_CONNECTION)) != 0;
    std::cout << std::to_string(passiveConnection) << std::endl;

    int connectPort = 0;

    if (!passiveConnection) {
        connectPort = std::stoi(getenv(CONNECT_PORT));
    }
    std::cout << std::to_string(connectPort) << std::endl;

    SimpleTCP simpleTcp;

    simpleTcp.initialize();
    simpleTcp.open(listenPort, passiveConnection, connectPort);

    while (true);
}