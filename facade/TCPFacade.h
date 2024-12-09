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
    static void send(
        SOCKET socket,
        unsigned char* buffer,
        int bufferLength,
        const sockaddr* address
    );
    static int receive(
        SOCKET socket,
        unsigned char* buffer,
        int bufferLength
    );
};



#endif //TCPFACADE_H
