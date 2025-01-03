//
// Created by o.narvatov on 11/26/2024.
//

#ifndef BYTEINSERTER_H
#define BYTEINSERTER_H
#include <cstdint>


class ByteInserter {
public:
    static void insert8BitInt(unsigned char* sendbuf, uint8_t bits);
    static void insert16BitInt(unsigned char* sendbuf, uint16_t bits);
    static void insert32BitInt(unsigned char* sendbuf, uint32_t bits);
};



#endif //BYTEINSERTER_H
