//
// Created by o.narvatov on 12/16/2024.
//

#include "TCPHeaderTestUtils.h"

#include "tcb/LocalConnection.h"

TCPHeader TCPHeaderTestUtils::createHeader(const uint32_t sequenceNumber, const uint32_t ackNumber) const {
    TCPHeader tcpHeader = TCPHeader::constructSendTCPHeader(localConnection);

    tcpHeader.sequenceNumber = sequenceNumber;
    tcpHeader.ackNumber = ackNumber;

    return tcpHeader;
}
