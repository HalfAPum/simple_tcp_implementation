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

#include "../facade/TCPFacade.h"
#include "../Constants.h"

//Could modify local port
void TransmissionControlBlock::processListeningSocketMessage(
    const IPv4Header &ipv4Header,
    const UDPHeader &udpHeader,
    const TCPHeader &tcpHeader
) {
    if (connectionSocket != INVALID_SOCKET) return;

    connectionSocket = localConnection->createLocalSocket(true);
    localConnection->createForeignSocketAddress(tcpHeader.sourcePort);

    assert(state == LISTEN);

    std::cout << "IS SYN??? " << tcpHeader.SYN << std::endl;
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

    //temp
    sTCPHeader.ACK = true;
    sTCPHeader.ackNumber = sTCPHeader.sequenceNumber + 100;

    sTCPHeader.fillSendBuffer(sendbuf);

    std::cout << "----------------------------------------------------------------------------" << std::endl;
    std::cout << "------------------------------------SEND-PACKET-----------------------------" << std::endl;


    // //Calculate TCP checksum
    // sTCPHeader.calculateChecksum(sIPv4Header, sendbuf + IP_HEADER_LENGTH);

    //DEGUB Verify header
    TCPHeader::parseTCPHeader(sendbuf).print();

    TCPFacade::send(connectionSocket, sendbuf, SEND_TCP_HEADER_LENGTH, localConnection->foreignSockaddrr);
    int recv = TCPFacade::receive(connectionSocket, sendbuf, SEND_TCP_HEADER_LENGTH);
    std::cout<<"LOL? recv " << recv << std::endl;
    TCPHeader::parseTCPHeader(sendbuf).print();
}
