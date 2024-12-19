//
// Created by o.narvatov on 11/24/2024.
//

#ifndef TCPHEADER_H
#define TCPHEADER_H
#include <cstdint>
#include <iosfwd>
#include <istream>

#include "../ipv4/IPv4Header.h"

struct LocalConnection;
constexpr int SEND_TCP_HEADER_LENGTH = 28;

//Options
constexpr int END_OF_OPTION_LIST_OPTION_KIND = 0x00;
constexpr int NO_OPTION_KIND = 0x01;
constexpr int MAX_SEGMENT_SIZE_OPTION_KIND = 0x02;

constexpr int MAX_SEGMENT_SIZE_OPTION_LENGTH = 0x04;

constexpr int DATA_OFFSET_WORD_LENGTH = 4;

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

    //Debug
    std::string printOptions;

    static TCPHeader parseTCPHeader(const unsigned char* recvbuf);
    void print() const;

    [[nodiscard]] static TCPHeader constructSendTCPHeader(const LocalConnection *localConnection);

    void fillSendBuffer(unsigned char* sendbuff) const;

    void calculateChecksum(const IPv4Header &ipv4Header, unsigned char* sendbuf);

    [[nodiscard]] int getDataOffsetBytes() const;

    bool operator==(const TCPHeader &other) const;
};



#endif //TCPHEADER_H
