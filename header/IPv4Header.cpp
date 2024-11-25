//
// Created by o.narvatov on 11/23/2024.
//

#include "IPv4Header.h"

#include <iostream>

#include "../ByteExtractor.h"

IPv4Header IPv4Header::parseIPv4Header(const char* recvbuf) {
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
    const char* flagsByte = recvbuf + 6;
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

void IPv4Header::fillSendBuffer(char *sendbuf) {

}
