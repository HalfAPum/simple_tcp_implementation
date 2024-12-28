//
// Created by o.narvatov on 12/27/2024.
//

#ifndef TCPTESTUTILS_H
#define TCPTESTUTILS_H
#include "facade/TCPFacadeMock.h"
#include "header/tcp/TCPHeader.h"
#include "tcb/TransmissionControlBlock.h"
#include "TestConstants.h"
#include "message/base/TCPMessageStateMachine.h"


namespace TCPTestUtils {

    inline TCPHeader processIncomingTCPSegment(const TCPHeader &mockHeader, TransmissionControlBlock *tcb) {
        const auto mockFacade = dynamic_cast<TCPFacadeMock*>(TCPFacade::singleton);

        mockFacade->addToReceiveMessageQueue(mockHeader, false);
        TCPMessageStateMachine::singleton->processUDPMessage(TEST_SOCKET, tcb);

        return mockFacade->popFromSendSendMessageQueue();
    }

    inline TCPHeader processIncomingRawTCPSegment(
        const TCPHeader &mockHeader,
        std::unordered_map<uint16_t,TransmissionControlBlock*> &tcbMap
    ) {
        const auto mockFacade = dynamic_cast<TCPFacadeMock*>(TCPFacade::singleton);

        mockFacade->addToReceiveMessageQueue(mockHeader, true);
        TCPMessageStateMachine::singleton->processRawIPMessage(TEST_SOCKET, tcbMap);

        return mockFacade->popFromSendSendMessageQueue();
    }

    inline std::pair<uint16_t, TransmissionControlBlock*> createTCB(uint16_t port, const bool passive) {
        return {
            port,
            new TransmissionControlBlock(
                new LocalConnection(inet_addr(ADDR_TO_BIND), port), passive, DEFAULT_TIMEOUT
            )
        };
    }

}


#endif //TCPTESTUTILS_H
