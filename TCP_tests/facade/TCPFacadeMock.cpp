//
// Created by o.narvatov on 12/16/2024.
//

#include "TCPFacadeMock.h"

#include "Constants.h"
#include "../header/ipv4/IPv4HeaderMock.h"

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
    receiveMessageQueue.pop();

    const auto receiveBuffer = receiveMessage.first;
    const auto receiveBufferSize = receiveMessage.second;

    printf("FUCK DO IT FOR BUFF SIZE");
    printf(std::to_string(receiveBufferSize).c_str());
    for (int i = 0; i < receiveBufferSize; ++i) {
        buffer[i] = receiveBuffer[i];
    }

    return receiveBufferSize;
}

void TCPFacadeMock::addToReceiveMessageQueue(const TCPHeader &tcpHeader, const bool addUDPIPHeaders) {
    unsigned char buffer[BUFFLEN] {};

    int offset = 0;

    if (addUDPIPHeaders) {
        IPv4HeaderMock::fillSendBuffer(buffer);
        offset += IP_HEADER_LENGTH;
        //Ignore UDP info it's not used now anyway
        offset += UDP_HEADER_LENGTH;
    }

    tcpHeader.fillSendBuffer(buffer + offset);

    receiveMessageQueue.emplace(buffer, offset + SEND_TCP_HEADER_LENGTH);
    blockReceiveAdd = false;
}

TCPHeader TCPFacadeMock::popFromSendSendMessageQueue() {
    while (blockSendPop) {
        Sleep(0);
    }

    blockSendPop = true;

    auto front = sendMessageQueue.front();
    sendMessageQueue.pop();

    return front;
}