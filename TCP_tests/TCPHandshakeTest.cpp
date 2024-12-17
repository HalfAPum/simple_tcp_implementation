//
// Created by o.narvatov on 12/10/2024.
//
#include <iostream>
#include <catch2/catch_all.hpp>
#include <catch2/internal/catch_preprocessor_internal_stringify.hpp>

#include "SimpleTCP.h"
#include "facade/TCPFacadeMock.h"
#include "header/tcp/TCPHeaderTestUtils.h"

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


    SECTION("New connection for closed port") {
        TCPHeaderTestUtils tcpHeaderTestUtils(ACTIVE_LOCAL_PORT, ACTIVE_FOREIGN_PORT_CLOSED);

        auto synHeader = tcpHeaderTestUtils.createHeader(TransmissionControlBlock::generateISS());
        synHeader.SYN = true;

        mockFacade->addToReceiveMessageQueue(synHeader, true);

        TCPHeader sendTCPHeader = mockFacade->popFromSendSendMessageQueue();

        // SECTION("ACK bit is ON") {
        //     REQUIRE(sendTCPHeader.ACK);
        //     REQUIRE(sendTCPHeader.sequenceNumber == receiveTCPHeader.ackNumber);
        // }

        SECTION("ACK bit is OFF") {
            REQUIRE(sendTCPHeader.ACK);
            REQUIRE(sendTCPHeader.RST);

            REQUIRE(sendTCPHeader.sequenceNumber == 0);
            auto segLen = SEND_TCP_HEADER_LENGTH - synHeader.getDataOffsetBytes();
            REQUIRE(sendTCPHeader.ackNumber == synHeader.sequenceNumber + segLen);
        }
    }

    delete mockFacade;
}