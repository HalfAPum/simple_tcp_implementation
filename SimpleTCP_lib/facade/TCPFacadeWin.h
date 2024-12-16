//
// Created by o.narvatov on 12/16/2024.
//

#ifndef TCPFACADEWIN_H
#define TCPFACADEWIN_H
#include "TCPFacade.h"

class TCPFacadeWin final : public TCPFacade {
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
};

#endif //TCPFACADEWIN_H
