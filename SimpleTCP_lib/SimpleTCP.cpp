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
#include "TCPErrorMessages.h"
#include "facade/base/TCPFacade.h"
#include "message/TCPMessageStateMachineImpl.h"
#include "socket/base/SocketFactory.h"

constexpr auto LISTEN_THREAD_NAME = "LISTEN_THREAD";

bool checkResultFail(const bool result, const std::string &actionName, const SOCKET socket) {
    if (!result) return false;

    std::cout << actionName << " failed with error: " << WSAGetLastError() << std::endl;

    closesocket(socket);

    return true;
}

bool SimpleTCP::initialize(
    TCPMessageStateMachine *tcpMessageStateMachine,
    TCPFacade *tcpFacade,
    SocketFactory* socketFactory
) {
    TCPMessageStateMachine::initialize(tcpMessageStateMachine);
    TCPFacade::initialize(tcpFacade);
    SocketFactory::initialize(socketFactory);

    WSAData wsaData = {};

    //Initialize winsock
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup faield with error: " << iResult << std::endl;
        return false;
    }

    //Initialize listening socket
    listenSocket = SocketFactory::singleton->createUDPSocket(SOCK_RAW, true, DEFAULT_TCP_PORT);

    if (!setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, nullptr, 0)) {
        perror("setsockopt");
        throw std::exception();
    }

    std::thread listenThread(&SimpleTCP::listenNewConnections, this);
    listenThread.detach();

    return true;
}

void SimpleTCP::listenNewConnections() {
    while (true) {
        TCPMessageStateMachine::singleton->processRawIPMessage(listenSocket, tcbMap);
    }
}


LocalConnection SimpleTCP::open(
    const uint16_t localPort,
    const uint16_t foreignPort,
    const bool passive,
    const unsigned timeout
) {
    auto *localConnection = new LocalConnection(inet_addr(ADDR_TO_BIND), localPort);

    const auto tcbIt = tcbMap.find(localPort);

    if (tcbIt == tcbMap.end()) {
        //TCB doesn't exist (i.e., CLOSED STATE)
        tcbMap.emplace(localPort, new TransmissionControlBlock(localConnection, passive, timeout));

        if (!passive) {
            TCPMessageStateMachine::singleton->sendSYNMessage(localPort, foreignPort, tcbMap);
        }

        return *localConnection;
    }

    //TCB exists
    auto tcb = tcbIt->second;
    if (tcb->state != LISTEN) {
        std::cout << "error:  connection already exists" << std::endl;
        throw std::exception();
    }

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

std::string SimpleTCP::errorMessage = tcpError::NO_ERROR_M;