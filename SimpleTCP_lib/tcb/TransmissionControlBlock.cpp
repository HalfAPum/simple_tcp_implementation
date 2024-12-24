//
// Created by o.narvatov on 11/24/2024.
//

#include "TransmissionControlBlock.h"

#include <cassert>
#include <atomic>
#include <iostream>
#include <thread>
#include <winsock2.h>
#include <random>
#include <ws2tcpip.h>

#include "facade/base/TCPFacade.h"
#include "message/base/TCPMessageStateMachine.h"

void TransmissionControlBlock::processPacketListenState(const TCPHeader &tcpHeader) {
    localConnection->createForeignSocketAddress(tcpHeader.sourcePort);

    auto sendHeader = TCPHeader::constructSendTCPHeader(localConnection);

    if (tcpHeader.ACK) {
        sendHeader.sequenceNumber = tcpHeader.ackNumber;
        sendHeader.RST = true;

        sendTCPSegment(sendHeader);
    } else if (tcpHeader.SYN) {
        rcv_nxt = tcpHeader.sequenceNumber + 1;
        irs = tcpHeader.sequenceNumber;
        iss = generateISS();
        sendHeader.sequenceNumber = iss;
        sendHeader.ackNumber = rcv_nxt;
        sendHeader.SYN = true;
        sendHeader.ACK = true;

        sendTCPSegment(sendHeader);

        snd_nxt = iss + 1;
        snd_una = iss;
        state = SYN_RECEIVED;

        launchTCBThread();
    }
}

//Could modify local port
void TransmissionControlBlock::processListeningSocketMessage(const TCPHeader &tcpHeader) {
    assert(state == LISTEN);

    if (tcpHeader.RST) return;

    if (connectionSocket == INVALID_SOCKET) {
        connectionSocket = localConnection->createLocalSocket(true);
    }

    processPacketListenState(tcpHeader);
}

//Modifies local port
void TransmissionControlBlock::sendSYN(const uint16_t foreignPort) {
    connectionSocket = localConnection->createLocalSocket(true);
    localConnection->createForeignSocketAddress(foreignPort);

    TCPHeader header = TCPHeader::constructSendTCPHeader(localConnection);

    //Configure header
    iss = generateISS();
    header.sequenceNumber = iss;
    header.SYN = true;

    sendTCPSegment(header);

    //Configure TCB
    snd_una = iss;
    snd_nxt = iss + 1;
    state = SYN_SENT;
}

//Use pseudorandom number generator to generate initial segment sequence
uint32_t TransmissionControlBlock::generateISS() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, UINT32_MAX);

    return distrib(gen);
}

void TransmissionControlBlock::sendTCPSegment(TCPHeader &sTCPHeader) {
    unsigned char sendbuf[SEND_TCP_HEADER_LENGTH];

    sTCPHeader.fillSendBuffer(sendbuf);

    std::cout << "----------------------------------------------------------------------------" << std::endl;
    std::cout << "------------------------------------SEND-PACKET-----------------------------" << std::endl;


    // //Calculate TCP checksum
    // sTCPHeader.calculateChecksum(sIPv4Header, sendbuf + IP_HEADER_LENGTH);

    //DEGUB Verify header
    // TCPHeader::parseTCPHeader(sendbuf).print();

    TCPFacade::singleton->send(connectionSocket, sendbuf, SEND_TCP_HEADER_LENGTH, localConnection->foreignSockaddrr);
}

void TransmissionControlBlock::launchTCBThread() {
    if (threadLaunched) return;

    threadLaunched = true;

    std::thread thread(&TransmissionControlBlock::launchTCBThreadInternal, this);
    thread.detach();
}

void TransmissionControlBlock::launchTCBThreadInternal() {
    while (true) {
        TCPMessageStateMachine::singleton->processUDPMessage(connectionSocket, this);
    }
}
