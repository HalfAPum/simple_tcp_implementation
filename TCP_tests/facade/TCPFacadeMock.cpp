//
// Created by o.narvatov on 12/16/2024.
//

#include "TCPFacadeMock.h"

#include "Constants.h"

int TCPFacadeMock::receive(const SOCKET socket, unsigned char *buffer, const int bufferLength) {
    const int packetLength = realFacade->receive(socket, buffer, bufferLength);

    //Ignore non-TCP messages
    if (packetLength < TCP_SEGMENT_MIN_LENGTH) {
        const int bufferOffset = getSockType(socket) == SOCK_RAW ? IP_HEADER_LENGTH + UDP_HEADER_LENGTH : 0;
        receiveMessageQueue.emplace(TCPHeader::parseTCPHeader(buffer + bufferOffset));
    }

    return packetLength;
}

void TCPFacadeMock::send(const SOCKET socket, unsigned char *buffer, const int bufferLength, const sockaddr *address) {
    sendMessageQueue.emplace(TCPHeader::parseTCPHeader(buffer));

    realFacade->send(socket, buffer, bufferLength, address);
}

int TCPFacadeMock::getSockType(const SOCKET socket) {
    int optVal;
    int optLen = sizeof(int);

    getsockopt(
        socket,
        SOL_SOCKET,
        SO_TYPE,
        reinterpret_cast<char *>(&optVal),
        &optLen
    );

    printf(std::to_string(optVal).c_str());
    return optVal;
}


