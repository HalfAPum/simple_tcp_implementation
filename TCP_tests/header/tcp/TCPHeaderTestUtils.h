//
// Created by o.narvatov on 12/16/2024.
//

#ifndef TCPHEADERMOCK_H
#define TCPHEADERMOCK_H
#include "header/tcp/TCPHeader.h"
#include "tcb/LocalConnection.h"


class TCPHeaderTestUtils {
    LocalConnection *localConnection;
public:
    TCPHeaderTestUtils(
        const uint16_t _sourcePort,
        const uint16_t _destinationPort
    ) {
        localConnection = new LocalConnection(0, _sourcePort);
        localConnection->foreignPort = _destinationPort;
    }

    [[nodiscard]] TCPHeader createHeader(uint32_t sequenceNumber = 0, uint32_t ackNumber = 0) const;

    static void validate(TCPHeader &expectedHeader, TCPHeader &realHeader);

    ~TCPHeaderTestUtils() {
        delete localConnection;
    }
};



#endif //TCPHEADERMOCK_H