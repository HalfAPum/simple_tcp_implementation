//
// Created by o.narvatov on 12/19/2024.
//

#ifndef TCPERRORMESSAGES_H
#define TCPERRORMESSAGES_H
#include <string>

namespace tcpError {
    const std::string NO_ERROR_M = "no error";
    const std::string CONNECTION_RESET = "error: connection reset";
    const std::string CONNECTION_REFUSED = "error: connection refused";
}

#endif //TCPERRORMESSAGES_H
