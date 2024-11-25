
#include "SimpleTCP.h"

constexpr unsigned DEFAULT_PORT = 8080;

int main() {
    SimpleTCP simpleTcp;

    simpleTcp.initialize();
    simpleTcp.open(DEFAULT_PORT, true);

    while (true);
}