//
// Created by o.narvatov on 11/24/2024.
//

#ifndef TRANSMISSIONCONTROLBLOCK_H
#define TRANSMISSIONCONTROLBLOCK_H
#include <thread>
#include <winsock2.h>

#include "LocalConnection.h"
#include "State.h"
#include "header/TCPHeader.h"


class TransmissionControlBlock {
    std::thread thread;

    void processSegment(const TCPHeader &tcpHeader);

    void run();
public:
    const LocalConnection localConnection;
    bool passive;
    const unsigned timeout;
    SOCKET socket;
    State state = CLOSED;

    explicit TransmissionControlBlock(
        const LocalConnection _localConnection,
        const bool _passive,
        const unsigned _timeout,
        SOCKET _socket
    ) : localConnection(_localConnection),
        passive(_passive),
        timeout(_timeout),
        socket(_socket)
        {}

    void start();

};



#endif //TRANSMISSIONCONTROLBLOCK_H
