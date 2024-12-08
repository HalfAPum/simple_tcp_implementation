//
// Created by o.narvatov on 11/24/2024.
//

#include "SimpleTCP.h"

#include <cassert>
#include <iostream>
#include <map>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <psdk_inc/_wsadata.h>
#include <thread>

#include "Constants.h"
#include "header/udp/UDPHeader.h"

constexpr auto LISTEN_THREAD_NAME = "LISTEN_THREAD";

bool checkResultFail(const bool result, const std::string &actionName, const SOCKET socket) {
    if (!result) return false;

    std::cout << actionName << " failed with error: " << WSAGetLastError() << std::endl;

    closesocket(socket);

    return true;
}

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

bool SimpleTCP::initialize() {
    WSAData wsaData = {};

    //Initialize winsock
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup faield with error: " << iResult << std::endl;
        return false;
    }

    //Initialize listening socket
    listenSocket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (listenSocket == INVALID_SOCKET) {
        std::cout << "socket creation faield with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        throw std::exception();
    }

    if (!setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, nullptr, 0)) {
        perror("setsockopt");
        throw std::exception();
    }

    sockaddr_in sockstr {};
    sockstr.sin_addr.s_addr = inet_addr(ADDR_TO_BIND);
    sockstr.sin_family = AF_INET;
    sockstr.sin_port = htons(DEFAULT_TCP_PORT);
    constexpr auto sockstr_size = static_cast<socklen_t>(sizeof(sockstr));

    iResult = bind(listenSocket, reinterpret_cast<sockaddr *>(&sockstr), sockstr_size);
    if (checkResultFail(iResult == SOCKET_ERROR, "bind", listenSocket)) {
        throw std::exception();
    }

    std::thread listenThread(&SimpleTCP::listenNewConnections, this);
    listenThread.detach();

    return true;
}

void SimpleTCP::listenNewConnections() {
    while (true) {
        unsigned char recvbuf[BUFFLEN];

        const int recvResult = recv(listenSocket, reinterpret_cast<char*>(recvbuf), BUFFLEN, 0);

        if (checkResultFail(recvResult == RECV_ERROR, "recvResult", listenSocket)) {
            return;
        }

        //Packets are guaranteed to be have IP header since we use SOCK_RAW.
        auto ipv4Header = IPv4Header::parseIPv4Header(recvbuf);

        //Packets are also only UDP packets since we create socket on IPPROTO_UDP protocol
        assert(ipv4Header.protocol == IPPROTO_UDP);

        //Verify received Result size.
        //We should receive IP, UDP and TCP headers (each is at least 20, 8, 20 bytes long respectively).
        //Data payload is optional.
        if (validate(recvResult < TCP_SEGMENT_MIN_LENGTH,
            "Received message has size: " + std::to_string(recvResult) +
            ". This is less than Minimal TCP_SEGMENT_LENGTH " + std::to_string(TCP_SEGMENT_MIN_LENGTH)
        )) {
            continue;
        }

        auto udpHeader = UDPHeader::parseUDPHeader(recvbuf + IP_HEADER_LENGTH);
        auto tcpHeader = TCPHeader::parseTCPHeader(recvbuf + IP_HEADER_LENGTH + UDP_HEADER_LENGTH);

        auto tcbIt = tcbMap.find(tcpHeader.destinationPort);

        if (tcbIt == tcbMap.end()) {
            std::cout << "Ignore unknown packet to " << tcpHeader.destinationPort << std::endl;
            continue;
        }

        ipv4Header.print();
        udpHeader.print();
        tcpHeader.print();

        TransmissionControlBlock *tcb = tcbIt->second;
        tcb->processListeningSocketMessage(ipv4Header, udpHeader, tcpHeader);
    }
}


LocalConnection SimpleTCP::open(
    const uint16_t localPort,
    const uint16_t foreignPort,
    const bool passive,
    const unsigned timeout
) {
    auto *localConnection = new LocalConnection(inet_addr(ADDR_TO_BIND), localPort);

    const auto tcbIt = tcbMap.find(localConnection->localPort);

    if (tcbIt == tcbMap.end()) {
        //TCB doesn't exist (i.e., CLOSED STATE)
        auto *tcb = new TransmissionControlBlock(localConnection, passive, timeout);
        tcbMap[localConnection->localPort] = tcb;

        if (passive) {
            // tcb->connectionSocket = localConnection->createLocalSocket(false);
            //
            // unsigned char recvbuf[BUFFLEN];
            //
            // struct sockaddr_in SenderAddr;
            // int SenderAddrSize = sizeof (SenderAddr);
            //
            // int recv = recvfrom(tcb->connectionSocket, (char*)recvbuf, BUFFLEN, 0,(SOCKADDR*) &SenderAddr,&SenderAddrSize);
            // std::cout<<"FUCK RECEIVE " << recv << std::endl;
        } else {
            tcb->sendSYN(foreignPort);
        }

        return *localConnection;
    }

    //TCB exists
    auto tcb = tcbIt->second;
    if (tcb->state != LISTEN) {
        std::cout << "error:  connection already exists" << std::endl;
        throw std::exception();
    }

    //TODO
    /*
        * If active and the foreign socket is specified, then change the
      connection from passive to active, select an ISS.  Send a SYN
      segment, set SND.UNA to ISS, SND.NXT to ISS+1.  Enter SYN-SENT
      state.  Data associated with SEND may be sent with SYN segment or
      queued for transmission after entering ESTABLISHED state.  The
      urgent bit if requested in the command must be sent with the data
      segments sent as a result of this command.  If there is no room to
      queue the request, respond with "error:  insufficient resources".
      If Foreign socket was not specified, then return "error:  foreign
      socket unspecified".
         */

    return *localConnection;
}

void SimpleTCP::send(
    const LocalConnection &localConnection,
    const char *buffer,
    unsigned byteCount,
    bool PSH,
    bool URG,
    unsigned timeout
) {
    const auto tcbIt = tcbMap.find(localConnection.localPort);

    //CLOSED STATE (i.e., TCB does not exist)
    if (tcbIt == tcbMap.end()) {
        std::cout << "error:  connection does not exist" << std::endl;
        return;
    }

    auto tcb = tcbIt->second;

    if (tcb->state == LISTEN) {
        //TODO
    }

    //TODO
}

ReceiveParams SimpleTCP::receive(
    LocalConnection &localConnection,
    char *buffer,
    unsigned byteCount
) {
    const auto tcbIt = tcbMap.find(localConnection.localPort);

    //CLOSED STATE (i.e., TCB does not exist)
    if (tcbIt == tcbMap.end()) {
        std::cout << "error:  connection does not exist" << std::endl;
        throw std::exception();
    }

    //TODO
}