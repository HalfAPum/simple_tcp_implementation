//
// Created by o.narvatov on 12/16/2024.
//

#include "TCPHeaderTestUtils.h"

#include "tcb/LocalConnection.h"

TCPHeader TCPHeaderTestUtils::createHeader() const {
    auto tcpHeader = TCPHeader::constructSendTCPHeader(localConnection);

    return tcpHeader;
}

TCPHeader TCPHeaderTestUtils::noHeader() {
    const auto lc = new LocalConnection(0,0);

    auto tcpHeader = TCPHeader::constructSendTCPHeader(lc);

    delete lc;

    return tcpHeader;
}

