//
// Created by o.narvatov on 12/8/2024.
//

#ifndef TCPFACADE_H
#define TCPFACADE_H

#include <winsock2.h>


/**
 * Primary class resposibility is to send/receive messages and handle corresponding errors.
 */
struct TCPFacade {
    virtual ~TCPFacade() = default;

    virtual void send(
        SOCKET socket,
        unsigned char* buffer,
        int bufferLength,
        const sockaddr* address
    ) = 0;
    virtual int receive(
        SOCKET socket,
        unsigned char* buffer,
        int bufferLength
    ) = 0;


    static TCPFacade* singleton;

    static TCPFacade *initialize(TCPFacade* other);
};


#endif //TCPFACADE_H