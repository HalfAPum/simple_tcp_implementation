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

#include "SimpleTCP.h"
#include "TCPErrorMessages.h"
#include "facade/base/TCPFacade.h"
#include "message/base/TCPMessageStateMachine.h"


void TransmissionControlBlock::sendSYNACK(const TCPHeader &header) {
    auto sendHeader = TCPHeader::constructSendTCPHeader(localConnection);

    rcv_nxt = header.sequenceNumber + 1;
    irs = header.sequenceNumber;
    iss = generateISS();
    sendHeader.sequenceNumber = iss;
    sendHeader.ackNumber = rcv_nxt;
    sendHeader.SYN = true;
    sendHeader.ACK = true;

    sendTCPSegment(sendHeader);

    snd_nxt = iss + 1;
    snd_una = iss;
    state = SYN_RECEIVED;
}

void TransmissionControlBlock::sendRST(const TCPHeader &header) {
    auto sendHeader = TCPHeader::constructSendTCPHeader(localConnection);

    if (header.ACK) {
        sendHeader.sequenceNumber = header.ackNumber;
    } else {
        sendHeader.sequenceNumber = snd_nxt;
    }
    sendHeader.RST = true;

    sendTCPSegment(sendHeader);
}

void TransmissionControlBlock::sendEstablishedACK(const TCPHeader &header) {
    auto sendHeader = TCPHeader::constructSendTCPHeader(localConnection);

    rcv_nxt = header.sequenceNumber + 1;
    irs = header.sequenceNumber;
    snd_una = header.ackNumber;

    sendHeader.sequenceNumber = snd_nxt;
    sendHeader.ackNumber = rcv_nxt;
    sendHeader.ACK = true;

    sendTCPSegment(sendHeader);

    snd_una = snd_nxt;
    snd_nxt = snd_nxt + 1;
    state = ESTABLISHED;
}



void TransmissionControlBlock::processPacketListenState(const TCPHeader &header) {
    localConnection->createForeignSocketAddress(header.sourcePort);

    if (header.ACK) {
        sendRST(header);
    } else if (header.SYN) {
        sendSYNACK(header);

        launchTCBThread();
    }
}

//Could modify local port
void TransmissionControlBlock::processListeningSocketMessage(const TCPHeader &tcpHeader) {
    assert(state == LISTEN);

    if (tcpHeader.RST || tcpHeader.FIN) return;

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

    // //Calculate TCP checksum
    // sTCPHeader.calculateChecksum(sIPv4Header, sendbuf + IP_HEADER_LENGTH);

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

void TransmissionControlBlock::processSynSentSocketMessage(const TCPHeader &header) {
    if (header.ACK) {
        if (header.ackNumber <= iss || header.ackNumber > snd_nxt) {
            //ACK is NOT acceptable

            if (header.RST) {
                //Drop segment.
                return;
            }

            sendRST(header);

            return;
        }

        //ACK is acceptable

        if (header.RST) {
            SimpleTCP::errorMessage = tcpError::CONNECTION_RESET;
            state = CLOSED;
            return;
        }

        //Our SYN has been ACKed
        sendEstablishedACK(header);

        return;
    }

    if (header.SYN) {
        sendSYNACK(header);
    }
}

void TransmissionControlBlock::processSynReceivedSocketMessage(const TCPHeader &header) {
    if (header.RST) {
        if (passive) {
            state = LISTEN;
            return;
        }

        //Active
        SimpleTCP::errorMessage = tcpError::CONNECTION_REFUSED;
        state = CLOSED;
        return;
    }

    if (header.SYN) {
        sendRST(header);

        SimpleTCP::errorMessage = tcpError::CONNECTION_RESET;
        state = CLOSED;
        return;
    }

    if (header.ACK) {
        if (snd_una <= header.ackNumber && header.ackNumber <= snd_nxt) {
            state = ESTABLISHED;
        } else {
            sendRST(header);
            return;
        }
    }

    if (header.FIN) {
        state = CLOSE_WAIT;
    }
}