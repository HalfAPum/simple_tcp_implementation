//
// Created by o.narvatov on 12/27/2024.
//

#ifndef TCPSHAREDTESTS_H
#define TCPSHAREDTESTS_H

#include <set>
#include <vector>

#include "SimpleTCP.h"
#include "TCPErrorMessages.h"
#include "TCPTestUtils.h"
#include "catch2/catch_test_macros.hpp"
#include "header/tcp/TCPHeader.h"
#include "tcb/State.h"
inline std::set postIncludingSynReceivedList { SYN_RECEIVED, ESTABLISHED, FIN_WAITING_1, FIN_WAITING_2, CLOSE_WAIT, CLOSING, LAST_ACK, TIME_WAIT };

/**
  * SYN-RECEIVED
  * ESTABLISHED STATE
  * FIN-WAIT STATE-1
  * FIN-WAIT STATE-2
  * CLOSE-WAIT STATE
  * CLOSING STATE
  * LAST-ACK STATE
  * TIME-WAIT STATE
  **/
inline void SYNbitTest_PostIncludingSynReceived(TCPHeader &mockHeader, TransmissionControlBlock* tcb) {
  SECTION("SYN bit is ON") {
    REQUIRE(postIncludingSynReceivedList.find(tcb->state) != postIncludingSynReceivedList.end());

    mockHeader.SYN = true;

    TCPHeader implHeader = TCPTestUtils::processIncomingTCPSegment(mockHeader, tcb);

    REQUIRE(implHeader.RST);
    REQUIRE(implHeader.sequenceNumber == tcb->snd_nxt);

    REQUIRE(tcb->state == CLOSED);
    REQUIRE(SimpleTCP::errorMessage == tcpError::CONNECTION_RESET);
  }
}

#endif //TCPSHAREDTESTS_H