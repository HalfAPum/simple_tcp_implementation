//
// Created by o.narvatov on 11/23/2024.
//

#include "IPv4Header.h"

#include <iostream>

#include "../../byteop/extract/ByteExtractor.h"

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
