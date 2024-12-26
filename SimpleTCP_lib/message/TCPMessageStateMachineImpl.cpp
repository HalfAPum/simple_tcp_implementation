//
// Created by o.narvatov on 12/20/2024.
//

#include "TCPMessageStateMachineImpl.h"

#include <cassert>
#include <iostream>

#include "Constants.h"
#include "facade/base/TCPFacade.h"

/**
 * @param result boolean to evaluate validation.
 * @param message message to print in case of false validation.
 * @return false if validation failed, true otherwise.
 */
bool validate(const bool result, const std::string &message) {
    if (!result) return false;

    std::cout << message << std::endl;

    return true;
}

void TCPMessageStateMachineImpl::processRawIPMessage(
    const SOCKET listenSocket,
    std::unordered_map<uint16_t, TransmissionControlBlock *> &tcbMap
) {
    unsigned char recvbuf[BUFFLEN];

    int packetLength = TCPFacade::singleton->receive(listenSocket, recvbuf, BUFFLEN);

    //Verify received Result size.
    //We should receive IP, UDP and TCP headers (each is at least 20, 8, 20 bytes long respectively).
    //Data payload is optional.
    if (validate(packetLength < TCP_SEGMENT_MIN_LENGTH,
        "Received message has size: " + std::to_string(packetLength) +
        ". This is less than Minimal TCP_SEGMENT_LENGTH " + std::to_string(TCP_SEGMENT_MIN_LENGTH)
    )) {
        return;
    }

    //Packets are guaranteed to be have IP header since we use SOCK_RAW.
    auto ipv4Header = IPv4Header::parseIPv4Header(recvbuf);

    //Packets are also only UDP packets since we create socket on IPPROTO_UDP protocol
    assert(ipv4Header.protocol == IPPROTO_UDP);

    auto udpHeader = UDPHeader::parseUDPHeader(recvbuf + IP_HEADER_LENGTH);
    auto tcpHeader = TCPHeader::parseTCPHeader(recvbuf + IP_HEADER_LENGTH + UDP_HEADER_LENGTH);

    mutex_.lock();
    //Filter calls we make by ourself. Ignore comparing address since we test now only local calls.
    if (auto ownCallIt = tcbMap.find(tcpHeader.sourcePort); ownCallIt != tcbMap.end()) {
        mutex_.unlock();
        return;
    }
    mutex_.unlock();

    auto tcbIt = tcbMap.find(tcpHeader.destinationPort);

    if (tcbIt == tcbMap.end()) {
        //TCB does not exist (Connection state is closed).

        if (tcpHeader.RST) return;

        auto *rstConnection = new LocalConnection(inet_addr(ADDR_TO_BIND), tcpHeader.destinationPort);
        const SOCKET rstSocket = rstConnection->createLocalSocket(false);
        rstConnection->createForeignSocketAddress(tcpHeader.sourcePort);

        auto rstHeader = TCPHeader::constructSendTCPHeader(rstConnection);

        unsigned char sendbuf[SEND_TCP_HEADER_LENGTH];

        if (tcpHeader.ACK) {
            rstHeader.sequenceNumber = tcpHeader.ackNumber;
            rstHeader.RST = true;
        } else {
            rstHeader.sequenceNumber = 0;
            const int segLen = packetLength - IP_HEADER_LENGTH - UDP_HEADER_LENGTH - tcpHeader.getDataOffsetBytes();
            rstHeader.ackNumber = tcpHeader.sequenceNumber + segLen;
            rstHeader.RST = true;
            rstHeader.ACK = true;
        }

        rstHeader.fillSendBuffer(sendbuf);

        TCPFacade::singleton->send(rstSocket, sendbuf, SEND_TCP_HEADER_LENGTH, rstConnection->foreignSockaddrr);

        closesocket(rstSocket);
        delete rstConnection;

        return;
    }

    TransmissionControlBlock *tcb = tcbIt->second;
    tcb->processListeningSocketMessage(tcpHeader);
}

void TCPMessageStateMachineImpl::sendSYNMessage(
    const uint16_t localPort,
    const uint16_t foreignPort,
    std::unordered_map<uint16_t, TransmissionControlBlock*> &tcbMap
) {
    TransmissionControlBlock* tcb = tcbMap[localPort];

    mutex_.lock();
    tcb->sendSYN(foreignPort);
    tcbMap.erase(localPort);
    tcbMap.emplace(tcb->localConnection->localPort, tcb);
    tcb->launchTCBThread();
    mutex_.unlock();
}

void TCPMessageStateMachineImpl::processUDPMessage(
    const SOCKET connectionSocket,
    TransmissionControlBlock* tcb
) {
    unsigned char recvbuf[SEND_TCP_HEADER_LENGTH];

    int packetLength = TCPFacade::singleton->receive(connectionSocket, recvbuf, SEND_TCP_HEADER_LENGTH);

    auto recvHeader = TCPHeader::parseTCPHeader(recvbuf);

    const auto state = tcb->state;

    if (state == CLOSED || state == LISTEN || state == SYN_SENT) {
        if (recvHeader.FIN) return;
    }

    if (state == SYN_SENT) {
        tcb->processSynSentSocketMessage(recvHeader);
    } else if (state == SYN_RECEIVED) {
        tcb->processSynReceivedSocketMessage(recvHeader);
    }
}

