//
// Created by o.narvatov on 11/24/2024.
//

#ifndef TCPHEADER_H
#define TCPHEADER_H
#include <cstdint>


struct TCPHeader {
    uint16_t sourcePort;
    uint16_t destinationPort;
    uint32_t sequenceNumber;
    uint32_t ackNumber;
    uint8_t dataOffset;
    uint8_t reversed;
    bool URG;
    bool ACK;
    bool PSH;
    bool RST;
    bool SYN;
    bool FIN;
    uint16_t windowSize;
    uint16_t checksum;
    uint16_t urgentPointer;
    uint16_t segmentSize;

    static TCPHeader parseTCPHeader(const char* recvbuf);
};



#endif //TCPHEADER_H
