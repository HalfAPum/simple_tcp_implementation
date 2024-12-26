//
// Created by o.narvatov on 12/10/2024.
//
#include <iostream>
#include <catch2/catch_all.hpp>
#include <catch2/internal/catch_preprocessor_internal_stringify.hpp>

#include "SimpleTCP.h"
#include "TestConstants.h"
#include "TCPErrorMessages.h"
#include "facade/TCPFacadeMock.h"
#include "header/tcp/TCPHeaderTestUtils.h"
#include "socket/SocketFactoryMock.h"

constexpr uint16_t PASSIVE_LOCAL_PORT = 8080;
constexpr uint16_t ACTIVE_LOCAL_PORT = 9080;
constexpr uint16_t ACTIVE_FOREIGN_PORT = PASSIVE_LOCAL_PORT;
constexpr uint16_t ACTIVE_FOREIGN_PORT_CLOSED = PASSIVE_LOCAL_PORT + 1;

//Use it when value doesn't actually matter but should be present for test
constexpr int ANY_NUMBER = 123456;


void doAckBitOnTest(
    TCPHeader &mockHeader,
    TCPFacadeMock *mockFacade,
    std::unordered_map<uint16_t, TransmissionControlBlock*>& tcbMap
) {
    mockHeader.ACK = true;
    mockHeader.ackNumber = ANY_NUMBER;

    mockFacade->addToReceiveMessageQueue(mockHeader, true);
    TCPMessageStateMachine::singleton->processRawIPMessage(TEST_SOCKET, tcbMap);

    TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

    REQUIRE(implHeader.RST);
    REQUIRE(implHeader.sequenceNumber == mockHeader.ackNumber);
}

std::pair<uint16_t, TransmissionControlBlock*> createTCB(uint16_t port, const bool passive) {
    return {
        port,
        new TransmissionControlBlock(
            new LocalConnection(inet_addr(ADDR_TO_BIND), port), passive, DEFAULT_TIMEOUT
        )
    };
}

void doSegAckInvalidTest(
    const uint32_t ackNumber,
    TCPHeader &mockHeader,
    TCPFacadeMock *mockFacade,
    TransmissionControlBlock* tcb
) {
    mockHeader.ackNumber = ackNumber;

    mockFacade->addToReceiveMessageQueue(mockHeader, false);
    TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

    TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

    REQUIRE(implHeader.RST);
    REQUIRE(implHeader.sequenceNumber == mockHeader.ackNumber);
}


TEST_CASE("TCB_Closed") {
    auto* mockFacade = new TCPFacadeMock();
    auto* tcpMessageStateMachine = new TCPMessageStateMachineImpl();
    auto* socketFactory = new SocketFactoryMock();
    std::unordered_map<uint16_t, TransmissionControlBlock*> tcbMap{};

    TCPMessageStateMachine::initialize(tcpMessageStateMachine);
    TCPFacade::initialize(mockFacade);
    SocketFactory::initialize(socketFactory);

    TCPHeaderTestUtils tcpHeaderTestUtils(ACTIVE_LOCAL_PORT, ACTIVE_FOREIGN_PORT_CLOSED);

    auto mockHeader = tcpHeaderTestUtils.createHeader();

    SECTION("ACK bit is OFF") {
        mockHeader.ACK = false;
        mockHeader.sequenceNumber = ANY_NUMBER;

        mockFacade->addToReceiveMessageQueue(mockHeader, true);
        tcpMessageStateMachine->processRawIPMessage(TEST_SOCKET, tcbMap);

        TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

        REQUIRE(implHeader.ACK);
        REQUIRE(implHeader.RST);

        REQUIRE(implHeader.sequenceNumber == 0);
        auto segLen = SEND_TCP_HEADER_LENGTH - mockHeader.getDataOffsetBytes();
        REQUIRE(implHeader.ackNumber == mockHeader.sequenceNumber + segLen);
    }

    SECTION("ACK bit is ON") {
        doAckBitOnTest(mockHeader, mockFacade, tcbMap);
    }
}

TEST_CASE("TCB_Listen") {
    auto* mockFacade = new TCPFacadeMock();
    auto* tcpMessageStateMachine = new TCPMessageStateMachineImpl();
    auto* socketFactory = new SocketFactoryMock();
    std::unordered_map<uint16_t, TransmissionControlBlock*> tcbMap{};

    TCPMessageStateMachine::initialize(tcpMessageStateMachine);
    TCPFacade::initialize(mockFacade);
    SocketFactory::initialize(socketFactory);

    auto tcb = createTCB(PASSIVE_LOCAL_PORT, true);
    tcbMap.emplace(tcb);

    TCPHeaderTestUtils tcpHeaderTestUtils(ACTIVE_LOCAL_PORT, ACTIVE_FOREIGN_PORT);

    auto mockHeader = tcpHeaderTestUtils.createHeader();

    SECTION("RST bit is ON") {
        mockHeader.RST = true;

        mockFacade->addToReceiveMessageQueue(mockHeader, true);
        tcpMessageStateMachine->processRawIPMessage(TEST_SOCKET, tcbMap);

        TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

        REQUIRE(implHeader == TCPHeaderTestUtils::noHeader());
    }

    SECTION("ACK bit is ON") {
        doAckBitOnTest(mockHeader, mockFacade, tcbMap);
    }

    SECTION("SYN bit is ON") {
        mockHeader.SYN = true;
        mockHeader.sequenceNumber = TransmissionControlBlock::generateISS();

        mockFacade->addToReceiveMessageQueue(mockHeader, true);
        tcpMessageStateMachine->processRawIPMessage(TEST_SOCKET, tcbMap);

        TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

        REQUIRE(implHeader.SYN);
        REQUIRE(implHeader.ACK);

        REQUIRE(implHeader.ackNumber == mockHeader.sequenceNumber + 1);

        REQUIRE(tcb.second->state == State::SYN_RECEIVED);
    }
}

TEST_CASE("TCB_SynSent") {
    auto* mockFacade = new TCPFacadeMock();
    auto* tcpMessageStateMachine = new TCPMessageStateMachineImpl();
    auto* socketFactory = new SocketFactoryMock();
    std::unordered_map<uint16_t, TransmissionControlBlock*> tcbMap{};

    TCPMessageStateMachine::initialize(tcpMessageStateMachine);
    TCPFacade::initialize(mockFacade);
    SocketFactory::initialize(socketFactory);

    SECTION("Verify segment from SYN-SENT TCB") {
        auto tcb = createTCB(ACTIVE_LOCAL_PORT, false);
        tcbMap.emplace(tcb);
        TCPMessageStateMachine::singleton->sendSYNMessage(ACTIVE_LOCAL_PORT, ACTIVE_FOREIGN_PORT, tcbMap);

        TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

        REQUIRE(implHeader.SYN);

        REQUIRE(tcb.second->state == State::SYN_SENT);
    }

    SECTION("Send segment to SYN-SENT TCB") {
        auto tcbPair = createTCB(ACTIVE_LOCAL_PORT, false);
        tcbMap.emplace(tcbPair);

        auto tcb = tcbPair.second;
        tcb->state = SYN_SENT;
        tcb->iss = TransmissionControlBlock::generateISS();
        tcb->snd_nxt = tcb->iss + 1;

        TCPHeaderTestUtils tcpHeaderTestUtils(PASSIVE_LOCAL_PORT, ACTIVE_LOCAL_PORT);

        auto mockHeader = tcpHeaderTestUtils.createHeader();

        SECTION("ACK bit is ON") {
            mockHeader.ACK = true;
            mockHeader.sequenceNumber = ANY_NUMBER;
            mockHeader.ackNumber = ANY_NUMBER;

            SECTION("SEG.ACK if off bounds") {
                SECTION("SEG.ACK is equal to ISS") {
                    doSegAckInvalidTest(tcb->iss, mockHeader, mockFacade, tcb);
                }

                SECTION("SEG.ACK is less then ISS") {
                    doSegAckInvalidTest(tcb->iss -3, mockHeader, mockFacade, tcb);
                }

                SECTION("SEG.ACK is bigger then SND.NXT") {
                    doSegAckInvalidTest(tcb->iss + 2, mockHeader, mockFacade, tcb);
                }
            }

            SECTION("RST bit is ON") {
                SECTION("SEG.ACK is acceptable") {
                    mockHeader.ackNumber = tcb->iss + 1;
                    mockHeader.RST = true;

                    mockFacade->addToReceiveMessageQueue(mockHeader, false);
                    TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

                    TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

                    REQUIRE(implHeader == TCPHeaderTestUtils::noHeader());
                    REQUIRE(tcb->state == CLOSED);
                    REQUIRE(SimpleTCP::errorMessage == tcpError::CONNECTION_RESET);
                }

                SECTION("SEG.ACK is NOT acceptable") {
                    mockHeader.ackNumber = tcb->iss;
                    mockHeader.RST = true;

                    mockFacade->addToReceiveMessageQueue(mockHeader, false);
                    TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

                    TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

                    REQUIRE(implHeader == TCPHeaderTestUtils::noHeader());
                }
            }
        }

        SECTION("ACK bit is OFF") {
            mockHeader.ACK = false;

            SECTION("RST bit is ON") {
                mockHeader.RST = true;

                mockFacade->addToReceiveMessageQueue(mockHeader, false);
                TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

                TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

                REQUIRE(implHeader == TCPHeaderTestUtils::noHeader());
            }
        }

        SECTION("Check SYN bit") {
            SECTION("SYN bit is ON") {
                mockHeader.SYN = true;
                mockHeader.sequenceNumber = TransmissionControlBlock::generateISS();

                SECTION("ACK bit is ON") {
                    SECTION("SEG.ACK is acceptable") {
                        mockHeader.ackNumber = tcb->iss + 1;
                        mockHeader.ACK = true;

                        mockFacade->addToReceiveMessageQueue(mockHeader, false);
                        TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

                        TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

                        REQUIRE(implHeader.ACK);
                        REQUIRE(implHeader.sequenceNumber == tcb->iss + 1);
                        REQUIRE(implHeader.ackNumber == mockHeader.sequenceNumber + 1);

                        REQUIRE(tcb->state == State::ESTABLISHED);
                    }

                    SECTION("SEG.ACK is NOT acceptable") {
                        mockHeader.ackNumber = tcb->iss;
                        mockHeader.ACK = true;

                        mockFacade->addToReceiveMessageQueue(mockHeader, false);
                        TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

                        TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

                        REQUIRE(implHeader.RST);
                        REQUIRE(implHeader.sequenceNumber == mockHeader.ackNumber);
                    }
                }

                SECTION("ACK bit of OFF") {
                    mockHeader.ACK = false;

                    mockFacade->addToReceiveMessageQueue(mockHeader, false);
                    TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

                    TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

                    REQUIRE(implHeader.SYN);
                    REQUIRE(implHeader.ACK);
                    REQUIRE(implHeader.ackNumber == mockHeader.sequenceNumber + 1);

                    REQUIRE(tcb->state == SYN_RECEIVED);
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
            TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

            TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

            REQUIRE(implHeader == TCPHeaderTestUtils::noHeader());
        }
    }
}

TEST_CASE("TCB_SynReceived") {
    auto* mockFacade = new TCPFacadeMock();
    auto* tcpMessageStateMachine = new TCPMessageStateMachineImpl();
    auto* socketFactory = new SocketFactoryMock();
    std::unordered_map<uint16_t, TransmissionControlBlock*> tcbMap{};

    TCPMessageStateMachine::initialize(tcpMessageStateMachine);
    TCPFacade::initialize(mockFacade);
    SocketFactory::initialize(socketFactory);

    auto tcbPair = createTCB(ACTIVE_LOCAL_PORT, true);
    tcbMap.emplace(tcbPair);

    auto tcb = tcbPair.second;
    tcb->state = SYN_RECEIVED;
    tcb->iss = TransmissionControlBlock::generateISS();
    tcb->snd_una = tcb->iss + 2;
    tcb->snd_nxt = tcb->iss + 3;

    TCPHeaderTestUtils tcpHeaderTestUtils(PASSIVE_LOCAL_PORT, ACTIVE_LOCAL_PORT);

    auto mockHeader = tcpHeaderTestUtils.createHeader();
    mockHeader.sequenceNumber = ANY_NUMBER;

    SECTION("RST bit is ON") {
        mockHeader.RST = true;

        SECTION("Connection was initiated as Passive") {
            tcb->passive = true;

            mockFacade->addToReceiveMessageQueue(mockHeader, false);
            TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

            TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

            REQUIRE(implHeader == TCPHeaderTestUtils::noHeader());
            REQUIRE(tcb->state == LISTEN);
        }

        SECTION("Connection was initated as Active") {
            tcb->passive = false;

            mockFacade->addToReceiveMessageQueue(mockHeader, false);
            TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

            TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

            REQUIRE(implHeader == TCPHeaderTestUtils::noHeader());
            REQUIRE(tcb->state == CLOSED);
            REQUIRE(SimpleTCP::errorMessage == tcpError::CONNECTION_REFUSED);
        }
    }

    SECTION("SYN bit is ON") {
        mockHeader.SYN = true;

        mockFacade->addToReceiveMessageQueue(mockHeader, false);
        TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

        TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

        REQUIRE(implHeader.RST);

        REQUIRE(tcb->state == CLOSED);
        REQUIRE(SimpleTCP::errorMessage == tcpError::CONNECTION_RESET);
    }

    SECTION("ACK bit is OFF") {
        mockHeader.ACK = false;

        mockFacade->addToReceiveMessageQueue(mockHeader, false);
        TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

        TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

        REQUIRE(implHeader == TCPHeaderTestUtils::noHeader());
    }

    SECTION("ACK bit is ON") {
        mockHeader.ACK = true;

        SECTION("SEG.ACK is acceptable") {
            mockHeader.ackNumber = tcb->snd_nxt;

            mockFacade->addToReceiveMessageQueue(mockHeader, false);
            TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

            TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

            REQUIRE(implHeader == TCPHeaderTestUtils::noHeader());
            REQUIRE(tcb->state == ESTABLISHED);
        }

        SECTION("SEG.ACK is NOT acceptable") {
            mockHeader.ackNumber = tcb->snd_una;

            mockFacade->addToReceiveMessageQueue(mockHeader, false);
            TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

            TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

            REQUIRE(implHeader.RST);
            REQUIRE(implHeader.sequenceNumber == mockHeader.ackNumber);
        }
    }

    SECTION("FIN, URG bits are ON") {
        mockHeader.FIN = true;
        mockHeader.URG = true;

        mockFacade->addToReceiveMessageQueue(mockHeader, false);
        TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

        TCPHeader implHeader = mockFacade->popFromSendSendMessageQueue();

        REQUIRE(implHeader == TCPHeaderTestUtils::noHeader());
    }
}