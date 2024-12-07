//
// Created by o.narvatov on 12/7/2024.
//

#ifndef UDPHEADER_H
#define UDPHEADER_H
#include <cstdint>


struct IPv4Header;

struct UDPHeader {
    uint16_t sourcePort;
    uint16_t destinationPort;
    uint16_t length;
    uint16_t checksum;

    static UDPHeader parseUDPHeader(const unsigned char* recvbuf);

    void calculateChecksum(const IPv4Header &ipv4Header, unsigned char* sendbuf);

};



#endif //UDPHEADER_H
