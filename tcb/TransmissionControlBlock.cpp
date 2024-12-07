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

#include "../Constants.h"

bool checkResultFail1(const bool result, const std::string &actionName, const SOCKET socket) {
    if (!result) return false;

    std::cout << actionName << " failed with error: " << WSAGetLastError() << std::endl;

    closesocket(socket);

    return true;
}

void printMessage(char buffer[BUFFLEN], const int size) {
    std::cout << "Message size: " << size << std::endl;

    for(int i = 0 ; i < size ; ++i){
        std::cout << buffer[i];
    }
    std::cout << std::endl;
    printf("\n");
}

void TransmissionControlBlock::processListeningSocketMessage(
    const IPv4Header &ipv4Header,
    const UDPHeader &udpHeader,
    const TCPHeader &tcpHeader
) {
    if (connectionSocket != INVALID_SOCKET) return;

    localConnection->createForeignSocketAddress(inet_addr(ADDR_TO_BIND), tcpHeader.sourcePort);

    assert(state == LISTEN);

    std::cout << "IS SYN??? " << tcpHeader.SYN << std::endl;
}


void TransmissionControlBlock::sendSYN(const uint16_t foreignPort) {
    connectionSocket = localConnection->createLocalSocket(true);
    localConnection->createForeignSocketAddress(inet_addr(ADDR_TO_BIND), foreignPort);

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



void TransmissionControlBlock::processSegment(const IPv4Header &receiveIPv4Header, const TCPHeader &receiveTCPHeader) {
    // IPv4Header sendIPv4Header = receiveIPv4Header.constructSendIPv4Header();
    // // TCPHeader sendTCPHeader = receiveTCPHeader.constructSendTCPHeader(localConnection->localPort);
    //
    // if (state == LISTEN) {
    //     if (receiveTCPHeader.RST) {
    //         // An incoming RST should be ignored.  Return.
    //         return;
    //     }
    //
    //     // Any acknowledgment is bad if it arrives on a connection still in
    //     // the LISTEN state.  An acceptable reset segment should be formed
    //     // for any arriving ACK-bearing segment.  The RST should be
    //     // formatted as follows:
    //     //   <SEQ=SEG.ACK><CTL=RST>
    //     // Return.
    //
    //     if (receiveTCPHeader.ackNumber != 0) {
    //         //TODO
    //     }
    //
    //     if (receiveTCPHeader.SYN) {
    //         std::cout << "YEAH! IT'S SYN!" << std::endl;
    //
    //         iss = generateISS();
    //         sendTCPHeader.sequenceNumber = iss;
    //         rcv_nxt = receiveTCPHeader.sequenceNumber + 1;
    //         sendTCPHeader.ackNumber = rcv_nxt;
    //         sendTCPHeader.SYN = true;
    //         sendTCPHeader.ACK = true;
    //
    //         snd_nxt = iss + 1;
    //         snd_una = iss;
    //
    //         state = SYN_RECEIVED;
    //
    //         localConnection->createForeignSocket(receiveIPv4Header.sourceIPAddress, receiveTCPHeader.sourcePort);
    //         connectionSocket = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
    //         if (connectionSocket == INVALID_SOCKET) {
    //             std::cout << "socket creation faield with error: " << WSAGetLastError() << std::endl;
    //             WSACleanup();
    //             throw std::exception();
    //         }
    //         const int iResult = connect(connectionSocket, localConnection->foreignSockaddrr, localConnection->foreignSockaddrrLength);
    //         if (checkResultFail1(iResult == SOCKET_ERROR, "connect", connectionSocket)) {
    //             WSACleanup();
    //             throw std::exception();
    //         }
    //
    //         sendTCPSegment(sendIPv4Header, sendTCPHeader);
    //     }
    // }
}

void TransmissionControlBlock::run() {
    unsigned char recvbuf[BUFFLEN];

    // do {
    //     const int recvResult = recv(listenSocket, reinterpret_cast<char*>(recvbuf), BUFFLEN, 0);
    //
    //     if (checkResultFail1(recvResult == RECV_ERROR, "recvResult", listenSocket)) {
    //         return;
    //     }
    //
    //     //Verify received Result size.
    //     //We should receive IP and TCP headers (each is at least 20 bytes long).
    //     //Data payload is optional.
    //     if (validate(recvResult < TCP_SEGMENT_MIN_LENGTH, listenSocket,
    //         "Received message has size: " + std::to_string(recvResult) +
    //         ". This is less than TCP_SEGMENT_LENGTH " + std::to_string(TCP_SEGMENT_MIN_LENGTH)
    //     )) {
    //         return;
    //     }
    //
    //     auto ipv4Header = IPv4Header::parseIPv4Header(recvbuf);
    //     auto tcpHeader = TCPHeader::parseTCPHeader(recvbuf + IP_HEADER_LENGTH);
    //
    //     if (tcpHeader.destinationPort != localConnection->localPort) {
    //         std::cout << "Ignore unknown packet to " << tcpHeader.destinationPort << std::endl;
    //         continue;
    //     }
    //
    //     processSegment(ipv4Header, tcpHeader);
    // } while((localConnection->localPort == 8080));
}

void TransmissionControlBlock::start() {
    std::thread thrd(run, this);
    thrd.detach();
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
    TCPHeader::parseTCPHeader(sendbuf);

    const int sendResult = sendto(connectionSocket, (char *)(sendbuf), SEND_TCP_HEADER_LENGTH, 0,
        localConnection->foreignSockaddrr, sizeof(*localConnection->foreignSockaddrr));
    checkResultFail1(sendResult == SOCKET_ERROR, "sendto", connectionSocket);
}
