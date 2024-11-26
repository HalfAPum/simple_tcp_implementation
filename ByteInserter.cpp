//
// Created by o.narvatov on 11/26/2024.
//

#include "ByteInserter.h"


void ByteInserter::insert8BitInt(char *sendbuf, const uint8_t bits) {
    sendbuf[0] = static_cast<char>(bits);
}

void ByteInserter::insert16BitInt(char *sendbuf, const uint16_t bits) {
    sendbuf[0] = static_cast<char>((bits >> 8) & 0xFF);
    sendbuf[1] = static_cast<char>(bits & 0xFF);
}

void ByteInserter::insert32BitInt(char *sendbuf, const uint32_t bits) {
    sendbuf[0] = static_cast<char>((bits >> 24) & 0xFFFF);
    sendbuf[1] = static_cast<char>((bits >> 16) & 0xFFFF);
    sendbuf[2] = static_cast<char>((bits >> 8) & 0xFFFF);
    sendbuf[3] = static_cast<char>(bits & 0xFFFF);
}