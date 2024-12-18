//
// Created by o.narvatov on 12/18/2024.
//

#include "IPv4HeaderMock.h"

#include <atomic>
#include <winsock2.h>

#include "byteop/insert/ByteInserter.h"
#include "header/ipv4/IPv4Header.h"

//Static uniqie value for each IP header
static std::atomic<uint64_t> ipv4Identification = 1;

IPv4Header constructSendIPv4Header() {
    IPv4Header sIPv4Header {};

    sIPv4Header.version = IPv4_Version;
    //We send IP header with no Options. Hence length is constant.
    sIPv4Header.headerLength = IPv4_HEADER_LENGTH;
    sIPv4Header.typeOfService = 0;

    //Total length cannot be calculated yet.
    //Calculate it when assembling packet.
    //uint16_t totalLength;

    sIPv4Header.identification = ipv4Identification.load();
    ++ipv4Identification;

    sIPv4Header.reserved = false;
    sIPv4Header.dontFragment = true;
    sIPv4Header.moreFragments = false;
    //We don't put any optional so offset is 0.
    sIPv4Header.fragmentOffset = 0;
    sIPv4Header.timeToLive = IPv4_TTL;
    sIPv4Header.protocol = IPPROTO_UDP;
    //For simplicity send 0 checksum.
    //Although you just need to calculate it after assembling the segment.
    sIPv4Header.headerChecksum = 0;
    //Ignore IP
    sIPv4Header.sourceIPAddress = 0;
    sIPv4Header.destinationIPAddress = 0;

    return sIPv4Header;
}

void IPv4HeaderMock::fillSendBuffer(unsigned char *sendbuf) {
    IPv4Header header = constructSendIPv4Header();

    //Combine version and headerLength into single byte
    ByteInserter::insert8BitInt(sendbuf, header.version << 4 | header.headerLength);
    ByteInserter::insert8BitInt(sendbuf + 1, header.typeOfService);

    //Use hardcoded 48 (20 for IP header and 28 for TCP header)
    //At this point it totally incorrect but we don't use it.
    //But it doesn't affet our tests.
    ByteInserter::insert16BitInt(sendbuf + 2, 48);
    ByteInserter::insert16BitInt(sendbuf + 4, header.identification);

    const uint8_t flags = header.reserved << 7 | header.dontFragment << 6 | header.moreFragments << 5;
    ByteInserter::insert8BitInt(sendbuf + 6, flags | header.fragmentOffset >> 8);
    ByteInserter::insert8BitInt(sendbuf + 7, header.fragmentOffset);
    ByteInserter::insert8BitInt(sendbuf + 8, header.timeToLive);
    ByteInserter::insert8BitInt(sendbuf + 9, header.protocol);
    //For now don't calculate. If required, then do calculations here.
    ByteInserter::insert16BitInt(sendbuf + 10, header.headerChecksum);
    ByteInserter::insert32BitInt(sendbuf + 12, header.sourceIPAddress);
    ByteInserter::insert32BitInt(sendbuf + 16, header.destinationIPAddress);
}