
#include "SimpleTCP.h"

constexpr uint16_t DEFAULT_PORT = 9080;

int main() {
    SimpleTCP simpleTcp;

    simpleTcp.initialize();
    simpleTcp.open(DEFAULT_PORT, true);

    while (true);
}