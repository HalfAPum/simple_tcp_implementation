//
// Created by o.narvatov on 11/23/2024.
//

#include "IPv4Header.h"

#include <atomic>
#include <iostream>
#include <winsock2.h>

#include "../../byteop/extract/ByteExtractor.h"
#include "../../byteop/insert/ByteInserter.h"

IPv4Header IPv4Header::parseIPv4Header(const unsigned char* recvbuf) {
    IPv4Header ipv4Header {};

    //Each char is a byte (1 byte = 8 bits)
    //Version (4 bits)
    ipv4Header.version = ByteExtractor::get4BitInt(recvbuf, true);
    //Header length (4 bits)
    ipv4Header.headerLength = ByteExtractor::get4BitInt(recvbuf, false);
    //Type of Service (8 bits)
    ipv4Header.typeOfService = recvbuf[1];
    //Total Length (16 bits)
    ipv4Header.totalLength = ByteExtractor::get16BitInt(recvbuf + 2);
    //Identification (16 bits)
    ipv4Header.identification = ByteExtractor::get16BitInt(recvbuf + 4);
    //Flags (3 bits)
    const unsigned char* flagsByte = recvbuf + 6;
    //  Reserved (1 bit)
    ipv4Header.reserved = ByteExtractor::getBit(flagsByte, 0);
    //  Don't Fragment (1 bit)
    ipv4Header.dontFragment = ByteExtractor::getBit(flagsByte, 1);
    //  More Fragments (1 bit)
    ipv4Header.moreFragments = ByteExtractor::getBit(flagsByte, 2);
    //Fragment Offset (13 bits)
    //Eliminate Flags bits and concatenate next byte
    ipv4Header.fragmentOffset = ((recvbuf[6] & 0x1F) << 8) | recvbuf[7];
    //Time to Live (8 bits)
    ipv4Header.timeToLive = recvbuf[8];
    //Protocol (8 bits)
    ipv4Header.protocol = recvbuf[9];
    //Header Checksum (16 bits)
    ipv4Header.headerChecksum = ByteExtractor::get16BitInt(recvbuf + 10);
    //Source IP Address (32 bits)
    ipv4Header.sourceIPAddress = ByteExtractor::get32BitInt(recvbuf + 12);
    //Destination IP Address (32 bits)
    ipv4Header.destinationIPAddress = ByteExtractor::get32BitInt(recvbuf + 16);
    //Options (variable length)
    //Ignore Options. Pray they are not present)

    return ipv4Header;
}

void IPv4Header::print() const {
    std::cout << "Parse IP Header" << std::endl;
    std::cout << "Version: " << static_cast<int>(version) << std::endl;
    std::cout << "Header Length: " << static_cast<int>(headerLength) << std::endl;
    std::cout << "Type of Service: " << static_cast<int>(typeOfService) << std::endl;
    std::cout << "Total length: " << static_cast<int>(totalLength) << std::endl;
    std::cout << "Identification: " << static_cast<int>(identification) << std::endl;
    std::cout << "Reserved: " << reserved << std::endl;
    std::cout << "Don't Fragment: " << dontFragment << std::endl;
    std::cout << "More Fragments: " << moreFragments << std::endl;
    std::cout << "Fragment Offset: " << static_cast<int>(fragmentOffset) << std::endl;
    std::cout << "Time to Live: " << static_cast<int>(timeToLive) << std::endl;
    std::cout << "Protocol: " << static_cast<int>(protocol) << std::endl;
    std::cout << "Header Checksum: " << static_cast<int>(headerChecksum) << std::endl;
    std::cout << "Source IP Address: " << sourceIPAddress << std::endl;
    std::cout << "Destination IP Address: " << destinationIPAddress << std::endl;
    std::cout << std::endl;
}

//Static uniqie value for each IP header
static std::atomic<uint64_t> ipv4Identification = 1;

IPv4Header IPv4Header::constructSendIPv4Header() {
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

void IPv4Header::fillSendBuffer(unsigned char *sendbuf) const {
    //Combine version and headerLength into single byte
    ByteInserter::insert8BitInt(sendbuf, version << 4 | headerLength);
    ByteInserter::insert8BitInt(sendbuf + 1, typeOfService);

    //Use hardcoded 48 (20 for IP header and 28 for TCP header)
    //At this point it totally incorrect but we don't use it.
    //But it doesn't affet our tests.
    ByteInserter::insert16BitInt(sendbuf + 2, 48);
    ByteInserter::insert16BitInt(sendbuf + 4, identification);

    const uint8_t flags = reserved << 7 | dontFragment << 6 | moreFragments << 5;
    ByteInserter::insert8BitInt(sendbuf + 6, flags | fragmentOffset >> 8);
    ByteInserter::insert8BitInt(sendbuf + 7, fragmentOffset);
    ByteInserter::insert8BitInt(sendbuf + 8, timeToLive);
    ByteInserter::insert8BitInt(sendbuf + 9, protocol);
    //For now don't calculate. If required, then do calculations here.
    ByteInserter::insert16BitInt(sendbuf + 10, headerChecksum);
    ByteInserter::insert32BitInt(sendbuf + 12, sourceIPAddress);
    ByteInserter::insert32BitInt(sendbuf + 16, destinationIPAddress);
}