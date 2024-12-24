//
// Created by o.narvatov on 12/16/2024.
//

#include "TCPFacadeMock.h"

#include "Constants.h"
#include "../TestConstants.h"
#include "../header/ipv4/IPv4HeaderMock.h"
#include "../header/tcp/TCPHeaderTestUtils.h"

void TCPFacadeMock::send(const SOCKET socket, unsigned char *buffer, const int bufferLength, const sockaddr *address) {
    sendMessageQueue.emplace(TCPHeader::parseTCPHeader(buffer));
}

int TCPFacadeMock::receive(const SOCKET socket, unsigned char *buffer, const int bufferLength) {
    //Prohibit receiving for sockets Created not in test code directly
    if (socket != TEST_SOCKET) {
        throw std::exception();
    }

    const auto receiveMessage = receiveMessageQueue.front();
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
}

TCPHeader TCPFacadeMock::popFromSendSendMessageQueue() {
    if (sendMessageQueue.empty()) {
        return TCPHeaderTestUtils::noHeader();
    }

    auto front = sendMessageQueue.front();
    sendMessageQueue.pop();

    return front;
}