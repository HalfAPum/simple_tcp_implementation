//
// Created by o.narvatov on 11/23/2024.
//

#include "IPv4Header.h"

#include <iostream>

IPv4Header IPv4Header::parseIPv4Header(const char* recvbuf) {
    IPv4Header ipv4Header {};

    //Each char is a byte (1 byte = 8 bits)

    std::cout << "Parse IP Header" << std::endl;

    //Version (4 bits)
    uint8_t byte = recvbuf[0];
    ipv4Header.version = (byte >> 4) & 0x0F;
    std::cout << "Version: " << static_cast<int>(ipv4Header.version) << std::endl;

    //Header length (4 bits)
    ipv4Header.headerLength = byte & 0x0F;
    std::cout << "Header Length: " << static_cast<int>(ipv4Header.headerLength) << std::endl;

    //Type of Service (8 bits)
    ipv4Header.typeOfService = recvbuf[1];
    std::cout << "Type of Service: " << static_cast<int>(ipv4Header.typeOfService) << std::endl;

    //Total Length (16 bits)
    ipv4Header.totalLength = (recvbuf[2] << 8) | recvbuf[3];
    std::cout << "Total length: " << static_cast<int>(ipv4Header.totalLength) << std::endl;

    //Identification (16 bits)
    ipv4Header.identification = (recvbuf[4] << 8) | recvbuf[5];
    std::cout << "Identification: " << static_cast<int>(ipv4Header.identification) << std::endl;

    //Flags (3 bits)
    byte = recvbuf[6];
    //  Reserved (1 bit)
    ipv4Header.reserved = byte & 0x80;
    std::cout << "Reserved: " << ipv4Header.reserved << std::endl;
    //  Don't Fragment (1 bit)
    ipv4Header.dontFragment = byte & 0x40;
    std::cout << "Don't Fragment: " << ipv4Header.dontFragment << std::endl;
    //  More Fragments (1 bit)
    ipv4Header.moreFragments = byte & 0x20;
    std::cout << "More Fragments: " << ipv4Header.moreFragments << std::endl;

    //Fragment Offset (13 bits)
    //Eliminate Flags bits
    byte = byte & 0x1F;
    ipv4Header.fragmentOffset = (byte << 8) | recvbuf[7];
    std::cout << "Fragment Offset: " << static_cast<int>(ipv4Header.fragmentOffset) << std::endl;

    //Time to Live (8 bits)
    ipv4Header.timeToLive = recvbuf[8];
    std::cout << "Time to Live: " << static_cast<int>(ipv4Header.timeToLive) << std::endl;

    //Protocol (8 bits)
    ipv4Header.protocol = recvbuf[9];
    std::cout << "Protocol: " << static_cast<int>(ipv4Header.protocol) << std::endl;

    //Header Checksum (16 bits)
    ipv4Header.headerChecksum = (recvbuf[10] << 8) | recvbuf[11];
    std::cout << "Header Checksum: " << static_cast<int>(ipv4Header.headerChecksum) << std::endl;

    //Source IP Address (32 bits)
    ipv4Header.sourceIPAddress = (recvbuf[12] << 24) | (recvbuf[13] << 16) | (recvbuf[14] << 8) | recvbuf[15];
    std::cout << "Source IP Address: " << static_cast<int>(ipv4Header.sourceIPAddress) << std::endl;

    //Destination IP Address (32 bits)
    ipv4Header.destinationIPAddress = (recvbuf[16] << 24) | (recvbuf[17] << 16) | (recvbuf[18] << 8) | recvbuf[19];
    std::cout << "Destination IP Address: " << static_cast<int>(ipv4Header.destinationIPAddress) << std::endl;

    //Options (variable length)
    //Ignore Options. Pray they are not present)

    std::cout << std::endl;

    return ipv4Header;
}
