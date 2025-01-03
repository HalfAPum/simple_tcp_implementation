//
// Created by o.narvatov on 11/23/2024.
//

#ifndef IPV4HEADER_H
#define IPV4HEADER_H
#include <cstdint>

constexpr auto IPv4_Version = 4;
constexpr auto IPv4_HEADER_LENGTH = 5;
constexpr auto IPv4_TTL = 60;

struct IPv4Header {
    uint8_t version;
    uint8_t headerLength;
    uint8_t typeOfService;
    uint16_t totalLength;
    uint16_t identification;
    bool reserved;
    bool dontFragment;
    bool moreFragments;
    uint16_t fragmentOffset;
    uint8_t timeToLive;
    uint8_t protocol;
    uint16_t headerChecksum;
    uint32_t sourceIPAddress;
    uint32_t destinationIPAddress;

    static IPv4Header parseIPv4Header(const unsigned char* recvbuf);

    void print() const;

};

#endif //IPV4HEADER_H
