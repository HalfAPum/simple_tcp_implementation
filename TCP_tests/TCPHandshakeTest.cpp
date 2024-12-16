//
// Created by o.narvatov on 12/10/2024.
//
#include <iostream>
#include <catch2/catch_all.hpp>
#include <catch2/internal/catch_preprocessor_internal_stringify.hpp>

#include "SimpleTCP.h"
#include "facade/TCPFacadeMock.h"

constexpr int PASSIVE_LOCAL_PORT = 8080;
constexpr int ACTIVE_LOCAL_PORT = 9080;
constexpr int ACTIVE_FOREIGN_PORT = PASSIVE_LOCAL_PORT;
constexpr int ACTIVE_FOREIGN_PORT_CLOSED = PASSIVE_LOCAL_PORT + 1;


TEST_CASE("TCP_handshake_test", "[Passive]") {
    auto* mockFacade = new TCPFacadeMock();

    SimpleTCP simpleTcp {};

    bool initialized = simpleTcp.initialize(mockFacade);

    REQUIRE(initialized);

    auto localConnection = simpleTcp.open(PASSIVE_LOCAL_PORT);

    auto &receiveQueue = mockFacade->receiveMessageQueue;
    auto &sendQueue = mockFacade->sendMessageQueue;

    SECTION("New connection for closed port") {
        simpleTcp.open(ACTIVE_LOCAL_PORT, ACTIVE_FOREIGN_PORT_CLOSED, false);

        TCPHeader receiveTCPHeader = receiveQueue.front();
        receiveQueue.pop();

        REQUIRE(receiveTCPHeader.SYN);
        REQUIRE(receiveTCPHeader.destinationPort != PASSIVE_LOCAL_PORT);

        TCPHeader sendTCPHeader = sendQueue.front();
        sendQueue.pop();

        SECTION("ACK bit is ON") {
            REQUIRE(sendTCPHeader.ACK);
            REQUIRE(sendTCPHeader.sequenceNumber == receiveTCPHeader.ackNumber);
        }

        // SECTION("ACK bit is OFF") {
        //
        // }
    }

    delete mockFacade;
}