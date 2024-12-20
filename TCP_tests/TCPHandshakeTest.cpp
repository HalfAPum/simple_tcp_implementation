//
// Created by o.narvatov on 12/10/2024.
//
#include <iostream>
#include <catch2/catch_all.hpp>
#include <catch2/internal/catch_preprocessor_internal_stringify.hpp>

#include "SimpleTCP.h"
#include "TCPErrorMessages.h"
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

void doSegAckInvalidTest(const uint32_t ackNumber ,TCPHeader &mockHeader, TCPFacadeMock *mockFacade) {
    mockHeader.ackNumber = ackNumber;

    mockFacade->addToReceiveMessageQueue(mockHeader, false);
    TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

    REQUIRE(implHeader.RST);
    REQUIRE(implHeader.sequenceNumber == mockHeader.ackNumber);
}


TEST_CASE("TCB_Closed") {
    auto* mockFacade = new TCPFacadeMock();

    SimpleTCP simpleTcp {};

    bool initialized = simpleTcp.initialize(mockFacade);

    REQUIRE(initialized);

    SECTION("Send segment for CLOSED TCB") {
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

    delete mockFacade;
}

TEST_CASE("TCB_Listen") {
    auto* mockFacade = new TCPFacadeMock();

    SimpleTCP simpleTcp {};

    bool initialized = simpleTcp.initialize(mockFacade);

    REQUIRE(initialized);

    SECTION("Send segment for LISTEN TCB") {
        auto localConnection = simpleTcp.open(PASSIVE_LOCAL_PORT);

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

TEST_CASE("TCB_SynSent") {
    auto* mockFacade = new TCPFacadeMock();

    SimpleTCP simpleTcp {};

    bool initialized = simpleTcp.initialize(mockFacade);

    REQUIRE(initialized);

    SECTION("Receive segment from SYN-SENT TCB") {
        auto localConnection = simpleTcp.open(ACTIVE_LOCAL_PORT, ACTIVE_FOREIGN_PORT, false);

        TCPHeaderTestUtils tcpHeaderTestUtils(PASSIVE_LOCAL_PORT, ACTIVE_LOCAL_PORT);

        TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

        REQUIRE(implHeader.SYN);
    }

    SECTION("Send segment to SYN-SENT TCB") {
        auto localConnection = simpleTcp.open(ACTIVE_LOCAL_PORT, ACTIVE_FOREIGN_PORT, false);

        TCPHeaderTestUtils tcpHeaderTestUtils(PASSIVE_LOCAL_PORT, ACTIVE_LOCAL_PORT);

        //Enter simpleTCP SYN-SENT state
        TCPHeader synSentHeader = mockFacade->popFromSendSendMessageQueue();

        auto mockHeader = tcpHeaderTestUtils.createHeader();

        SECTION("ACK bit is ON") {
            mockHeader.ACK = true;
            mockHeader.sequenceNumber = ANY_NUMBER;
            mockHeader.ackNumber = ANY_NUMBER;

            SECTION("SEG.ACK if off bounds") {
                SECTION("SEG.ACK is equal to ISS") {
                    doSegAckInvalidTest(synSentHeader.sequenceNumber, mockHeader, mockFacade);
                }

                SECTION("SEG.ACK is less then ISS") {
                    doSegAckInvalidTest(synSentHeader.sequenceNumber -3, mockHeader, mockFacade);
                }

                SECTION("SEG.ACK is bigger then SND.NXT") {
                    doSegAckInvalidTest(synSentHeader.sequenceNumber + 2, mockHeader, mockFacade);
                }
            }

            SECTION("RST bit is ON") {
                mockHeader.RST = true;

                mockFacade->addToReceiveMessageQueue(mockHeader, false);

                REQUIRE(simpleTcp.getErrorMessage() == tcpError::CONNECTION_RESET);
            }
        }

        SECTION("ACK bit is OFF") {
            mockHeader.ACK = false;

            SECTION("RST bit is ON") {
                mockHeader.RST = true;

                mockFacade->addToReceiveMessageQueue(mockHeader, false);
                TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

                REQUIRE(implHeader == TCPHeaderTestUtils::noHeader());
            }
        }

        SECTION("Check SYN bit") {
            mockHeader.ackNumber = synSentHeader.sequenceNumber + 1;

            SECTION("SYN bit is ON") {
                mockHeader.SYN = true;
                mockHeader.sequenceNumber = TransmissionControlBlock::generateISS();

                SECTION("ACK bit is ON") {
                    mockHeader.ACK = true;

                    mockFacade->addToReceiveMessageQueue(mockHeader, false);
                    TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

                    REQUIRE(implHeader.ACK);
                    REQUIRE(implHeader.sequenceNumber == synSentHeader.sequenceNumber + 1);
                    REQUIRE(implHeader.ackNumber == mockHeader.sequenceNumber + 1);

                    //Connection established
                }

                SECTION("ACK bit of OFF") {
                    mockHeader.ACK = false;

                    mockFacade->addToReceiveMessageQueue(mockHeader, false);
                    TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

                    REQUIRE(implHeader.SYN);
                    REQUIRE(implHeader.ackNumber == mockHeader.sequenceNumber + 1);
                }
            }
        }

        //Technically we covered it before with ACK OFF
        //But it doesn't hurt to check it explicitly anyway
        SECTION("SYN, ACK, RST control bits are OFF") {
            mockHeader.SYN = false;
            mockHeader.ACK = false;
            mockHeader.RST = false;

            mockFacade->addToReceiveMessageQueue(mockHeader, false);
            TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

            REQUIRE(implHeader == TCPHeaderTestUtils::noHeader());
        }
    }

    delete mockFacade;
}