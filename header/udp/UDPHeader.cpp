//
// Created by o.narvatov on 12/7/2024.
//

#include "UDPHeader.h"

#include <iostream>

#include "../../byteop/extract/ByteExtractor.h"


UDPHeader UDPHeader::parseUDPHeader(const unsigned char *recvbuf) {
    UDPHeader header {};

    //Source Port (16 bits)
    header.sourcePort = ByteExtractor::get16BitInt(recvbuf + 0);
    //Destination Port (16 bits)
    header.destinationPort = ByteExtractor::get16BitInt(recvbuf + 2);
    //Length (16 bits)
    header.length = ByteExtractor::get16BitInt(recvbuf + 4);
    //Checksum (16 bits)
    header.checksum = ByteExtractor::get16BitInt(recvbuf + 6);

    return header;
}

void UDPHeader::print() const {
    std::cout << "Parse UDP header" << std::endl;
    std::cout << "Source port: " << static_cast<int>(sourcePort) << std::endl;
    std::cout << "Destination port: " << static_cast<int>(destinationPort) << std::endl;
    std::cout << "Length: " << static_cast<int>(length) << std::endl;
    std::cout << "Checksum: " << static_cast<int>(checksum) << std::endl;
    std::cout << std::endl;
}
