//
// Created by o.narvatov on 11/24/2024.
//

#include "SimpleTCP.h"

#include <iostream>
#include <map>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <psdk_inc/_wsadata.h>

#define ADDR_TO_BIND "127.0.0.1"

bool SimpleTCP::initialize() {
    WSAData wsaData = {};

    //Initialize winsock
    const int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup faield with error: " << iResult << std::endl;
        return false;
    }

    return true;
}

bool checkResultFail(const bool result, const std::string &actionName, const SOCKET socket) {
    if (!result) return false;

    std::cout << actionName << " failed with error: " << WSAGetLastError() << std::endl;

    closesocket(socket);

    return true;
}

LocalConnection SimpleTCP::open(const unsigned localPort, bool passive, unsigned timeout) {
    auto *localConnection = new LocalConnection(localPort, inet_addr(ADDR_TO_BIND));

    const auto tcbIt = tcbMap.find(localConnection->getTCBKey());

    if (tcbIt == tcbMap.end()) {
        //TCB doesn't exist (i.e., CLOSED STATE)

        //Try IPPROTO_RAW when succeed with other protocol
        const SOCKET listenSocket = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
        if (listenSocket == INVALID_SOCKET) {
            std::cout << "socket creation faield with error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            throw std::exception();
        }

        if (!setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEADDR, nullptr, 0)) {
            perror("setsockopt");
            throw std::exception();
        }

        sockaddr_in sockstr {};
        sockstr.sin_addr.s_addr = inet_addr(ADDR_TO_BIND);
        sockstr.sin_family = AF_INET;
        sockstr.sin_port = localConnection->localPort;
        constexpr auto sockstr_size = static_cast<socklen_t>(sizeof(sockstr));

        const auto iResult = bind(listenSocket, reinterpret_cast<sockaddr *>(&sockstr), sockstr_size);
        if (checkResultFail(iResult == SOCKET_ERROR, "bind", listenSocket)) {
            throw std::exception();
        }

        auto *tcb = new TransmissionControlBlock(localConnection, passive, timeout, listenSocket);
        tcbMap[localConnection->getTCBKey()] = tcb;

        if (passive) {
            tcb->state = LISTEN;
            tcb->start();
            return *localConnection;
        } else {
            //TODO
        }
    } else {
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
    const auto tcbIt = tcbMap.find(localConnection.getTCBKey());

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
    const auto tcbIt = tcbMap.find(localConnection.getTCBKey());

    //CLOSED STATE (i.e., TCB does not exist)
    if (tcbIt == tcbMap.end()) {
        std::cout << "error:  connection does not exist" << std::endl;
        throw std::exception();
    }

    //TODO
}