//
// Created by o.narvatov on 12/20/2024.
//

#ifndef TCPMESSAGESTATEMACHINEIMPL_H
#define TCPMESSAGESTATEMACHINEIMPL_H
#include <mutex>
#include <unordered_map>

#include "base/TCPMessageStateMachine.h"
#include "tcb/TransmissionControlBlock.h"


class TCPMessageStateMachineImpl final : public TCPMessageStateMachine {
    std::mutex mutex_;

public:
    void processRawIPMessage(
        SOCKET listenSocket,
        std::unordered_map<uint16_t, TransmissionControlBlock*> &tcbMap
    ) override;

    void sendSYNMessage(
        uint16_t localPort,
        uint16_t foreignPort,
        std::unordered_map<uint16_t, TransmissionControlBlock*> &tcbMap
    ) override;

    void processUDPMessage(
        SOCKET connectionSocket,
        TransmissionControlBlock* tcb
    ) override;
};



#endif //TCPMESSAGESTATEMACHINEIMPL_H
