//
// Created by o.narvatov on 12/16/2024.
//

#include "TCPHeaderTestUtils.h"

#include "tcb/LocalConnection.h"

TCPHeader TCPHeaderTestUtils::createHeader() const {
    TCPHeader tcpHeader = TCPHeader::constructSendTCPHeader(localConnection);

    return tcpHeader;
}
