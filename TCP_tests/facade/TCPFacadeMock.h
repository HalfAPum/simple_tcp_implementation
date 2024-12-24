//
// Created by o.narvatov on 12/16/2024.
//

#ifndef TCPFACADEMOCK_H
#define TCPFACADEMOCK_H
#include <queue>

#include "../header/tcp/TCPHeaderMockParams.h"
#include "facade/base/TCPFacade.h"
#include "header/tcp/TCPHeader.h"


class TCPFacadeMock final : public TCPFacade {
    std::queue<std::pair<unsigned char*, TCPHeaderMockParams>> receiveMessageQueue {};
    std::queue<TCPHeader> sendMessageQueue {};
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

    void addToReceiveMessageQueue(const TCPHeader &tcpHeader, bool toRawSock = false);
    TCPHeader popFromSendSendMessageQueue();

};


#endif //TCPFACADEMOCK_H
