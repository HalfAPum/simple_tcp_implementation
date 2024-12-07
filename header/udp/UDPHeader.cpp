//
// Created by o.narvatov on 12/7/2024.
//

#include "UDPHeader.h"

#include <iostream>

#include "../../byteop/extract/ByteExtractor.h"


UDPHeader UDPHeader::parseUDPHeader(const unsigned char *recvbuf) {
    UDPHeader header {};

    std::cout << "Parse UDP header" << std::endl;

    //Source Port (16 bits)
    header.sourcePort = ByteExtractor::get16BitInt(recvbuf + 0);
    std::cout << "Source port: " << static_cast<int>(header.sourcePort) << std::endl;

    //Destination Port (16 bits)
    header.destinationPort = ByteExtractor::get16BitInt(recvbuf + 2);
    std::cout << "Destination port: " << static_cast<int>(header.destinationPort) << std::endl;

    //Length (16 bits)
    header.length = ByteExtractor::get16BitInt(recvbuf + 4);
    std::cout << "Length: " << static_cast<int>(header.length) << std::endl;

    //Checksum (16 bits)
    header.checksum = ByteExtractor::get16BitInt(recvbuf + 6);
    std::cout << "Checksum: " << static_cast<int>(header.checksum) << std::endl;

    return header;
}
