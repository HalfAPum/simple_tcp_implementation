//
// Created by o.narvatov on 12/24/2024.
//

#ifndef TESTCONSTANTS_H
#define TESTCONSTANTS_H

#include <psdk_inc/_socket_types.h>


// Actual socket value doesn't matter because we mock all winsock calls.
constexpr SOCKET TEST_SOCKET = 0;

constexpr uint16_t PASSIVE_LOCAL_PORT = 8080;
constexpr uint16_t ACTIVE_LOCAL_PORT = 9080;
constexpr uint16_t ACTIVE_FOREIGN_PORT = PASSIVE_LOCAL_PORT;
constexpr uint16_t ACTIVE_FOREIGN_PORT_CLOSED = PASSIVE_LOCAL_PORT + 1;

//Use it when value doesn't actually matter but should be present for test
constexpr int ANY_NUMBER = 123456;

#endif //TESTCONSTANTS_H
