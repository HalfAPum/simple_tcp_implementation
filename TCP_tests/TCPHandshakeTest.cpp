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

//Use it when value doesn't actually matter but should be present for test
constexpr int ANY_NUMBER = 123456;

void doAckBitOnTest(TCPHeader &mockHeader, TCPFacadeMock *mockFacade) {
    mockHeader.ACK = true;
    mockHeader.ackNumber = ANY_NUMBER;

    mockFacade->addToReceiveMessageQueue(mockHeader, true);
    TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

    REQUIRE(implHeader.RST);
    REQUIRE(implHeader.sequenceNumber == mockHeader.ackNumber);
}

/**
 * This TEST_CASE covers all possible communication scenarious between TCP's
 * until the connection is etablished.
 */
TEST_CASE("TCP_handshake_test", "[Passive]") {
    auto* mockFacade = new TCPFacadeMock();

    SimpleTCP simpleTcp {};

    bool initialized = simpleTcp.initialize(mockFacade);

    REQUIRE(initialized);

    SECTION("Send segment for CLOSED port") {
        TCPHeaderTestUtils tcpHeaderTestUtils(ACTIVE_LOCAL_PORT, ACTIVE_FOREIGN_PORT_CLOSED);

        auto mockHeader = tcpHeaderTestUtils.createHeader();

        SECTION("ACK bit is OFF") {
            mockHeader.ACK = false;
            mockHeader.sequenceNumber = ANY_NUMBER;

            mockFacade->addToReceiveMessageQueue(mockHeader, true);
            TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

            REQUIRE(implHeader.ACK);
            REQUIRE(implHeader.RST);

            REQUIRE(implHeader.sequenceNumber == 0);
            auto segLen = SEND_TCP_HEADER_LENGTH - mockHeader.getDataOffsetBytes();
            REQUIRE(implHeader.ackNumber == mockHeader.sequenceNumber + segLen);
        }

        SECTION("ACK bit is ON") {
            doAckBitOnTest(mockHeader, mockFacade);
        }
    }

    auto localConnection = simpleTcp.open(PASSIVE_LOCAL_PORT);

    SECTION("Send segment for LISTEN port") {
        TCPHeaderTestUtils tcpHeaderTestUtils(ACTIVE_LOCAL_PORT, ACTIVE_FOREIGN_PORT);

        auto mockHeader = tcpHeaderTestUtils.createHeader();

        SECTION("RST bit is ON") {
            mockHeader.RST = true;

            mockFacade->addToReceiveMessageQueue(mockHeader, true);
            TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

            REQUIRE(implHeader == TCPHeaderTestUtils::noHeader());
        }

        SECTION("ACK bit is ON") {
            doAckBitOnTest(mockHeader, mockFacade);
        }

        SECTION("SYN bit is ON") {
            mockHeader.SYN = true;
            mockHeader.sequenceNumber = TransmissionControlBlock::generateISS();

            mockFacade->addToReceiveMessageQueue(mockHeader, true);
            TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

            REQUIRE(implHeader.SYN);
            REQUIRE(implHeader.ACK);

            REQUIRE(implHeader.ackNumber == mockHeader.sequenceNumber + 1);
        }
    }

    delete mockFacade;
}