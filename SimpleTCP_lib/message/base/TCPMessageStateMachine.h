//
// Created by o.narvatov on 12/20/2024.
//

#ifndef TCPMESSAGESTATEMACHINE_H
#define TCPMESSAGESTATEMACHINE_H
#include <cstdint>
#include <mutex>
#include <unordered_map>

#include "tcb/TransmissionControlBlock.h"


struct TCPMessageStateMachine {
    virtual ~TCPMessageStateMachine() = default;

    virtual void processRawIPMessage(
        SOCKET listenSocket,
        std::unordered_map<uint16_t, TransmissionControlBlock*> &tcbMap
    ) {}

    virtual void sendSYNMessage(
        uint16_t localPort,
        uint16_t foreignPort,
        std::unordered_map<uint16_t, TransmissionControlBlock*> &tcbMap
    ) {}

    virtual void processUDPMessage(
        SOCKET connectionSocket,
        TransmissionControlBlock* tcb
    ) {}

    static TCPMessageStateMachine* singleton;

    static TCPMessageStateMachine *initialize(TCPMessageStateMachine* other);
};



#endif //TCPMESSAGESTATEMACHINE_H
