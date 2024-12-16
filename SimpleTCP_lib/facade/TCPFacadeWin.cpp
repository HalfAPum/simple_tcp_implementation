//
// Created by o.narvatov on 12/16/2024.
//

#include "TCPFacadeWin.h"
#include "../Constants.h"

#include <iostream>


void TCPFacadeWin::send(
    const SOCKET socket,
    unsigned char* buffer,
    const int bufferLength,
    const sockaddr* address
) {
    const int sendResult = sendto(
        socket,
        reinterpret_cast<char *>(buffer),
        bufferLength,
        0,
        address,
        sizeof(*address)
    );

    if (sendResult == SOCKET_ERROR) {
        std::cout << "Couldn't send message, error occured: " << WSAGetLastError() << std::endl;
    }
}

int TCPFacadeWin::receive(const SOCKET socket, unsigned char *buffer, const int bufferLength) {
    const int recvResult = recv(socket, reinterpret_cast<char*>(buffer), bufferLength, 0);

    if (recvResult == RECV_ERROR) {
        std::cout << "recvResult failed with error: " << WSAGetLastError() << std::endl;

        closesocket(socket);

        throw std::exception();
    }

    return recvResult;
}