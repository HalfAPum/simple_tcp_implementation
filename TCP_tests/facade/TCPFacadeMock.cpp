//
// Created by o.narvatov on 12/16/2024.
//

#include "TCPFacadeMock.h"

#include "Constants.h"
#include "../header/ipv4/IPv4HeaderMock.h"
#include "../header/tcp/TCPHeaderTestUtils.h"

void TCPFacadeMock::send(const SOCKET socket, unsigned char *buffer, const int bufferLength, const sockaddr *address) {
    sendMessageQueue.emplace(TCPHeader::parseTCPHeader(buffer));
    blockSendPop = false;
}

int TCPFacadeMock::receive(const SOCKET socket, unsigned char *buffer, const int bufferLength) {
    //TODO this is done only for listening thread upgrade it when start testing at least one socket connection or more.
    while (blockReceiveAdd) {
        Sleep(0);
    }

    blockReceiveAdd = true;

    const auto receiveMessage = receiveMessageQueue.front();
    if (getSockType(socket) == SOCK_RAW && receiveMessage.second.ignoreRawSock) {
        blockReceiveAdd = false;
        return 0;
    }
    receiveMessageQueue.pop();

    const auto receiveBuffer = receiveMessage.first;
    const auto receiveBufferSize = receiveMessage.second.bufferSize;

    for (int i = 0; i < receiveBufferSize; ++i) {
        buffer[i] = receiveBuffer[i];
    }

    return receiveBufferSize;
}

void TCPFacadeMock::addToReceiveMessageQueue(const TCPHeader &tcpHeader, const bool toRawSock) {
    unsigned char buffer[BUFFLEN] {};

    int offset = 0;

    if (toRawSock) {
        IPv4HeaderMock::fillSendBuffer(buffer);
        offset += IP_HEADER_LENGTH;
        //Ignore UDP info it's not used now anyway
        offset += UDP_HEADER_LENGTH;
    }

    tcpHeader.fillSendBuffer(buffer + offset);

    receiveMessageQueue.emplace(buffer, TCPHeaderMockParams(offset + SEND_TCP_HEADER_LENGTH, !toRawSock));
    blockReceiveAdd = false;
}

TCPHeader TCPFacadeMock::popFromSendSendMessageQueue() {
    int cnt = 0;
    while (blockSendPop) {
        if (cnt > 2) {
            //No message arrived.
            return TCPHeaderTestUtils::noHeader();
        }

        Sleep(10);
        cnt++;
    }

    blockSendPop = true;

    auto front = sendMessageQueue.front();
    sendMessageQueue.pop();

    return front;
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