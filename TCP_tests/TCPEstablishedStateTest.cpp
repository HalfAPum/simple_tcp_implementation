//
// Created by o.narvatov on 12/27/2024.
//


#include "SimpleTCP.h"
#include "TCPErrorMessages.h"
#include "TCPSharedTests.h"
#include "catch2/catch_test_macros.hpp"
#include "facade/TCPFacadeMock.h"
#include "TCPTestUtils.h"
#include "TestConstants.h"
#include "header/tcp/TCPHeaderTestUtils.h"
#include "message/TCPMessageStateMachineImpl.h"
#include "socket/SocketFactoryMock.h"

using namespace TCPTestUtils;

TEST_CASE("TCP_Established") {
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
    tcb->state = ESTABLISHED;
    tcb->iss = TransmissionControlBlock::generateISS();
    tcb->snd_una = tcb->iss + 3;
    tcb->snd_nxt = tcb->iss + 3;

    TCPHeaderTestUtils tcpHeaderTestUtils(PASSIVE_LOCAL_PORT, ACTIVE_LOCAL_PORT);

    auto mockHeader = tcpHeaderTestUtils.createHeader();
    mockHeader.sequenceNumber = ANY_NUMBER;

    SECTION("RST bit is ON") {
        mockHeader.RST = true;

        TCPHeader implHeader = processIncomingTCPSegment(mockHeader, tcb);

        REQUIRE(implHeader == TCPHeaderTestUtils::noHeader());
        REQUIRE(tcb->state == CLOSED);
        REQUIRE(SimpleTCP::errorMessage == tcpError::CONNECTION_RESET);
    }

    SYNbitTest_PostIncludingSynReceived(mockHeader, tcb);
}