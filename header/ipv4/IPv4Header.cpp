//
// Created by o.narvatov on 11/23/2024.
//

#include "IPv4Header.h"

#include <atomic>
#include <iostream>

#include "../../byteop/extract/ByteExtractor.h"
#include "../../byteop/insert/ByteInserter.h"

IPv4Header IPv4Header::parseIPv4Header(const unsigned char* recvbuf) {
    IPv4Header ipv4Header {};

    //Each char is a byte (1 byte = 8 bits)

    std::cout << "Parse IP Header" << std::endl;

    //Version (4 bits)
    ipv4Header.version = ByteExtractor::get4BitInt(recvbuf, true);
    std::cout << "Version: " << static_cast<int>(ipv4Header.version) << std::endl;

    //Header length (4 bits)
    ipv4Header.headerLength = ByteExtractor::get4BitInt(recvbuf, false);
    std::cout << "Header Length: " << static_cast<int>(ipv4Header.headerLength) << std::endl;

    //Type of Service (8 bits)
    ipv4Header.typeOfService = recvbuf[1];
    std::cout << "Type of Service: " << static_cast<int>(ipv4Header.typeOfService) << std::endl;

    //Total Length (16 bits)
    ipv4Header.totalLength = ByteExtractor::get16BitInt(recvbuf + 2);
    std::cout << "Total length: " << static_cast<int>(ipv4Header.totalLength) << std::endl;

    //Identification (16 bits)
    ipv4Header.identification = ByteExtractor::get16BitInt(recvbuf + 4);
    std::cout << "Identification: " << static_cast<int>(ipv4Header.identification) << std::endl;

    //Flags (3 bits)
    const unsigned char* flagsByte = recvbuf + 6;
    //  Reserved (1 bit)
    ipv4Header.reserved = ByteExtractor::getBit(flagsByte, 0);
    std::cout << "Reserved: " << ipv4Header.reserved << std::endl;
    //  Don't Fragment (1 bit)
    ipv4Header.dontFragment = ByteExtractor::getBit(flagsByte, 1);
    std::cout << "Don't Fragment: " << ipv4Header.dontFragment << std::endl;
    //  More Fragments (1 bit)
    ipv4Header.moreFragments = ByteExtractor::getBit(flagsByte, 2);
    std::cout << "More Fragments: " << ipv4Header.moreFragments << std::endl;

    //Fragment Offset (13 bits)
    //Eliminate Flags bits and concatenate next byte
    ipv4Header.fragmentOffset = ((recvbuf[6] & 0x1F) << 8) | recvbuf[7];
    std::cout << "Fragment Offset: " << static_cast<int>(ipv4Header.fragmentOffset) << std::endl;

    //Time to Live (8 bits)
    ipv4Header.timeToLive = recvbuf[8];
    std::cout << "Time to Live: " << static_cast<int>(ipv4Header.timeToLive) << std::endl;

    //Protocol (8 bits)
    ipv4Header.protocol = recvbuf[9];
    std::cout << "Protocol: " << static_cast<int>(ipv4Header.protocol) << std::endl;

    //Header Checksum (16 bits)
    ipv4Header.headerChecksum = ByteExtractor::get16BitInt(recvbuf + 10);
    std::cout << "Header Checksum: " << static_cast<int>(ipv4Header.headerChecksum) << std::endl;

    //Source IP Address (32 bits)
    ipv4Header.sourceIPAddress = ByteExtractor::get32BitInt(recvbuf + 12);
    std::cout << "Source IP Address: " << ipv4Header.sourceIPAddress << std::endl;

    //Destination IP Address (32 bits)
    ipv4Header.destinationIPAddress = ByteExtractor::get32BitInt(recvbuf + 16);
    std::cout << "Destination IP Address: " << ipv4Header.destinationIPAddress << std::endl;

    //Options (variable length)
    //Ignore Options. Pray they are not present)

    std::cout << std::endl;

    return ipv4Header;
}

//Static uniqie value for each IP header
static std::atomic<uint64_t> ipv4Identification = 1;

IPv4Header IPv4Header::constructSendIPv4Header() const {
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
    sIPv4Header.protocol = IPv4_TCP_PROTOCOL;
    //For simplicity send 0 checksum.
    //Although you just need to calculate it after assembling the segment.
    sIPv4Header.headerChecksum = 0;
    sIPv4Header.sourceIPAddress = destinationIPAddress;
    sIPv4Header.destinationIPAddress = sourceIPAddress;

    return sIPv4Header;
}

void IPv4Header::fillSendBuffer(unsigned char *sendbuf) const {
    //Combine version and headerLength into single byte
    ByteInserter::insert8BitInt(sendbuf, version << 4 | headerLength);
    ByteInserter::insert8BitInt(sendbuf + 1, typeOfService);

    //TODO add data size to total length later
    //Use hardcoded 48 (20 for IP header and 28 for TCP header)
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
