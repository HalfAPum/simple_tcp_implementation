//
// Created by o.narvatov on 11/24/2024.
//

#include "ByteExtractor.h"

#include <iostream>
#include <stdexcept>


/*
 * Get bit from [recvbuf] at corresponding [index].
 * Example: we have 11101101 byte. getBit() function results will be:
 * getBit(byte, 0) = 1
 * getBit(byte, 1) = 1
 * getBit(byte, 2) = 1
 * getBit(byte, 3) = 0
 * ...
 */
bool ByteExtractor::getBit(const char *recvbuf, const unsigned index) {
    if (index > 7) {
        throw std::invalid_argument("ByteExtractor::getBit. Index parameter must be in 0 to 7 bound. "
                "Index parameter is " + std::to_string(index));
    }

    return (recvbuf[0] >> 7 - index) & 0x01;
}

/*
 * Uses 4 bits from [recvbuf] pointer.
 * Parameter [readLeadingBits] True means we read first 4 bits from current position of [recvbuf] pointer.
 * False means we read last 4 bits from current position of [recvbuf] pointer.
 */
uint8_t ByteExtractor::get4BitInt(const char *recvbuf, const bool readLeadingBits) {
    if (readLeadingBits) {
        return recvbuf[0] >> 4 & 0x0F;
    }

    return recvbuf[0] & 0x0F;
}

//Uses 2 bytes from [recvbuf] pointer.
uint16_t ByteExtractor::get16BitInt(const char *recvbuf) {
    std::cout << "HEX OF 16 bit in next line are " << std::hex << (int)recvbuf[0] << "+" << std::hex << (int)recvbuf[1] << " || ";
    std::cout << std::dec << (int)recvbuf[0] << "+" << std::dec << (int)recvbuf[1] << std::endl;

    uint16_t experimental = (recvbuf[0] << 8) | recvbuf[1];
    std::cout << "Experimental hex " << std::hex << (int)experimental << " || " << std::dec << (int)experimental << std::endl;
    return recvbuf[0] << 8 | recvbuf[1];
}

//Uses 4 bytes from [recvbuf] pointer.
uint32_t ByteExtractor::get32BitInt(const char *recvbuf) {
    return recvbuf[0] << 24 | recvbuf[1] << 16 | recvbuf[2] << 8 | recvbuf[3];
}