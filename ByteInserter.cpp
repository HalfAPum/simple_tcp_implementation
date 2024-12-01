//
// Created by o.narvatov on 11/26/2024.
//

#include "ByteInserter.h"


void ByteInserter::insert8BitInt(unsigned char *sendbuf, const uint8_t bits) {
    sendbuf[0] = static_cast<char>(bits);
}

void ByteInserter::insert16BitInt(unsigned char *sendbuf, const uint16_t bits) {
    sendbuf[0] = static_cast<char>(bits >> 8);
    sendbuf[1] = static_cast<char>(bits);
}

void ByteInserter::insert32BitInt(unsigned char *sendbuf, const uint32_t bits) {
    sendbuf[0] = static_cast<char>(bits >> 24);
    sendbuf[1] = static_cast<char>(bits >> 16);
    sendbuf[2] = static_cast<char>(bits >> 8);
    sendbuf[3] = static_cast<char>(bits);
}