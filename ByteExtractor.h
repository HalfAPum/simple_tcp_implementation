//
// Created by o.narvatov on 11/24/2024.
//

#ifndef BYTEEXTRACTOR_H
#define BYTEEXTRACTOR_H
#include <cstdint>


struct ByteExtractor {
    static bool getBit(const char* recvbuf, unsigned index);
    static uint8_t get4BitInt(const char* recvbuf, bool readLeadingBits);
    static uint16_t get16BitInt(const char* recvbuf);
    static uint32_t get32BitInt(const char* recvbuf);
};



#endif //BYTEEXTRACTOR_H
