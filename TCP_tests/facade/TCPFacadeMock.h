//
// Created by o.narvatov on 12/16/2024.
//

#ifndef TCPFACADEMOCK_H
#define TCPFACADEMOCK_H
#include <queue>

#include "facade/TCPFacade.h"
#include "header/tcp/TCPHeader.h"


class TCPFacadeMock final : public TCPFacade {
    static int getSockType(SOCKET socket);

    std::mutex mutex_;
public:
    std::queue<TCPHeader> receiveMessageQueue {};
    std::queue<TCPHeader> sendMessageQueue {};

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

    void addToReceiveMessageQueue(TCPHeader &tcpHeader);

};


#endif //TCPFACADEMOCK_H
