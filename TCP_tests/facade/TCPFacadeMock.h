//
// Created by o.narvatov on 12/16/2024.
//

#ifndef TCPFACADEMOCK_H
#define TCPFACADEMOCK_H
#include <atomic>
#include <queue>

#include "facade/TCPFacade.h"
#include "header/tcp/TCPHeader.h"


class TCPFacadeMock final : public TCPFacade {
    std::queue<std::pair<unsigned char*, int>> receiveMessageQueue {};
    std::queue<TCPHeader> sendMessageQueue {};

    //Block all Mock actions by default
    std::atomic<bool> blockReceiveAdd { true };
    std::atomic<bool> blockSendPop { true };
public:
    void send(
        SOCKET socket,
        unsigned char* buffer,
        int bufferLength,
        const sockaddr* address
    ) override;

    int receive(
        SOCKET socket,
        unsigned char* buffer,
        int bufferLength
    ) override;

    void addToReceiveMessageQueue(const TCPHeader &tcpHeader, bool addUDPIPHeaders = false);
    TCPHeader popFromSendSendMessageQueue();

};


#endif //TCPFACADEMOCK_H
