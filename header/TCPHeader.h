//
// Created by o.narvatov on 11/24/2024.
//

#ifndef TCPHEADER_H
#define TCPHEADER_H
#include <cstdint>

#include "../LocalConnection.h"

struct TCPHeader {
    uint16_t sourcePort;
    uint16_t destinationPort;
    uint32_t sequenceNumber;
    uint32_t ackNumber;
    uint8_t dataOffset;
    uint8_t reserved;
    bool URG;
    bool ACK;
    bool PSH;
    bool RST;
    bool SYN;
    bool FIN;
    uint16_t windowSize;
    uint16_t checksum;
    uint16_t urgentPointer;

    //Options
    uint16_t maxSegmentSizeOption;
    //Send-only options
    unsigned noOptionCountOption;
    bool endOfOptionListOption;

    static TCPHeader parseTCPHeader(const char* recvbuf);

    void fillSendBuffer(char* sendbuff, LocalConnection* localConnection);
};



#endif //TCPHEADER_H
