//
// Created by o.narvatov on 11/24/2024.
//

#include "TransmissionControlBlock.h"

#include <atomic>
#include <iostream>
#include <thread>
#include <winsock2.h>
#include <random>

#include "header/IPv4Header.h"

struct TCPHeader;
constexpr unsigned BUFFLEN = 65535;
constexpr auto RECV_ERROR = -1;

constexpr int IP_HEADER_LENGTH = 20;
constexpr int TCP_HEADER_MIN_LENGTH = 20;
constexpr int TCP_SEGMENT_MIN_LENGTH = IP_HEADER_LENGTH + TCP_HEADER_MIN_LENGTH;

constexpr int SEND_TCP_HEADER_LENGTH = 28;
constexpr int SEND_EMPTY_TCP_SEGMENT_LENGTH = IP_HEADER_LENGTH + SEND_TCP_HEADER_LENGTH;

//Static uniqie value for each IP header
static std::atomic<uint64_t> ipv4Identification = 1;

bool checkResultFail1(const bool result, const std::string &actionName, const SOCKET socket) {
    if (!result) return false;

    std::cout << actionName << " failed with error: " << WSAGetLastError() << std::endl;

    closesocket(socket);

    return true;
}

bool validate(const bool result, const SOCKET socket, const std::string &message) {
    if (!result) return false;

    std::cout << message << std::endl;

    closesocket(socket);

    return true;
}

void printMessage(char buffer[BUFFLEN], const int size) {
    std::cout << "Message size: " << size << std::endl;

    for(int i = 0 ; i < size ; ++i){
        std::cout << buffer[i];
    }
    std::cout << std::endl;
    printf("\n");
}

IPv4Header TransmissionControlBlock::constructSendIPv4Header(const IPv4Header &rIPv4Header, const TCPHeader &rTCPHeader) {
    IPv4Header sIPv4Header {};

    sIPv4Header.version = IPv4_Version;
    //We send IP header with no Options. Hence length is constant.
    sIPv4Header.headerLength = IPv4_HEADER_LENGTH;
    sIPv4Header.typeOfService = 0;

    //Total length cannot be calculated yet.
    //Calculate it when assembling packet.
    //uint16_t totalLength;

    sIPv4Header.identification = ipv4Identification.load();
    ++ipv4Identification;

    sIPv4Header.reserved = false;
    sIPv4Header.dontFragment = true;
    sIPv4Header.moreFragments = false;
    //We don't put any optional so offset is 0.
    sIPv4Header.fragmentOffset = 0;
    sIPv4Header.timeToLive = IPv4_TTL;
    sIPv4Header.protocol = IPv4_TCP_PROTOCOL;
    //For simplicity send 0 checksum.
    //Although you just need to calculate it after assembling the segment.
    sIPv4Header.headerChecksum = 0;
    sIPv4Header.sourceIPAddress = rIPv4Header.destinationIPAddress;
    sIPv4Header.destinationIPAddress = rIPv4Header.sourceIPAddress;

    return sIPv4Header;
}

TCPHeader TransmissionControlBlock::constructSendTCPHeader(const IPv4Header &rIPv4Header, const TCPHeader &rTCPHeader) {
    TCPHeader sTCPHeader {};

    sTCPHeader.sourcePort = localConnection->localPort;
    sTCPHeader.destinationPort = rTCPHeader.sourcePort;

    //No SEQ number for empty header.
    // uint32_t sequenceNumber;
    //No ACK Number for empty header.
    // uint32_t ackNumber;

    sTCPHeader.reserved = 0;
    sTCPHeader.URG = false;
    sTCPHeader.ACK = false;
    sTCPHeader.PSH = false;
    sTCPHeader.RST = false;
    sTCPHeader.SYN = false;
    sTCPHeader.FIN = false;

    //Set max window size we are ready to accept.
    sTCPHeader.windowSize = std::numeric_limits<uint16_t>::max();

    //Calculate checksum before sending segment.
    // uint16_t checksum;

    //No URG pointer for empty header.
    // uint16_t urgentPointer;

    //Set Max Segment Size.
    sTCPHeader.maxSegmentSizeOption = 0xFFFF;

    /*
     * Data offset is equal to TCP header size + options.
     * Options we send are Maximum Segment Size, No-Option, End of Option List.
     * TCP header must occupy bytes count multiples of 4 e.g. 20, 24, 28...
     * Max Segment Size Optin occupy 4 bytes.
     * Options must end with End of Option List Option which occupies 1 byte.
     * Add 3 additional No-Option Options (1 byte each) to make TCP header fit bytes count multiples of 4 rule.
     * Total byte count is 28 bytes.
     */
    sTCPHeader.noOptionCountOption = 3;
    sTCPHeader.endOfOptionListOption = true;

    //Data offset represents offset in bits.
    //Decrement 1 since dataOffset indicates end of header (indexing starts from 0).
    sTCPHeader.dataOffset = SEND_TCP_HEADER_LENGTH * 8 - 1;
}


void TransmissionControlBlock::processSegment(const IPv4Header &receiveIPv4Header, const TCPHeader &receiveTCPHeader) {
    IPv4Header sendIPv4Header = constructSendIPv4Header(receiveIPv4Header, receiveTCPHeader);
    TCPHeader sendTCPHeader = constructSendTCPHeader(receiveIPv4Header, receiveTCPHeader);

    if (state == LISTEN) {
        if (receiveTCPHeader.RST) {
            // An incoming RST should be ignored.  Return.
            return;
        }

        // Any acknowledgment is bad if it arrives on a connection still in
        // the LISTEN state.  An acceptable reset segment should be formed
        // for any arriving ACK-bearing segment.  The RST should be
        // formatted as follows:
        //   <SEQ=SEG.ACK><CTL=RST>
        // Return.

        if (receiveTCPHeader.ackNumber != 0) {
            //TODO
        }

        if (receiveTCPHeader.SYN) {
            std::cout << "YEAH! IT'S SYN!" << std::endl;

            iss = generateISS();
            sendTCPHeader.sequenceNumber = iss;
            rcv_nxt = receiveTCPHeader.sequenceNumber + 1;
            sendTCPHeader.ackNumber = rcv_nxt;
            sendTCPHeader.SYN = true;
            sendTCPHeader.ACK = true;

            snd_nxt = iss + 1;
            snd_una = iss;

            state = SYN_RECEIVED;

            //Fill foreign socket
            localConnection->foreignAddress = receiveIPv4Header.sourceIPAddress;
            localConnection->foreignPort = receiveTCPHeader.sourcePort;

            sendTCPSegment(sendIPv4Header, sendTCPHeader);
        }
    }
}

void TransmissionControlBlock::run() {
    char recvbuf[BUFFLEN];
    int recvbuflen = BUFFLEN;

    const int recvResult = recv(socket, recvbuf, recvbuflen, 0);

    if (checkResultFail1(recvResult == RECV_ERROR, "recvResult", socket)) {
        return;
    }

    //Verify received Result size.
    //We should receive IP and TCP headers (each is at least 20 bytes long).
    //Data payload is optional.
    if (validate(recvResult < TCP_SEGMENT_MIN_LENGTH, socket,
        "Received message has size: " + std::to_string(recvResult) +
        ". This is less than TCP_SEGMENT_LENGTH " + std::to_string(TCP_SEGMENT_MIN_LENGTH)
    )) {
        return;
    }

    auto ipv4Header = IPv4Header::parseIPv4Header(recvbuf);
    auto tcpHeader = TCPHeader::parseTCPHeader(recvbuf + IP_HEADER_LENGTH);

    processSegment(ipv4Header, tcpHeader);
}

void TransmissionControlBlock::start() {
    std::thread thrd(run, this);
    thrd.detach();
}

//Use pseudorandom number generator to generate initial segment sequence
uint32_t TransmissionControlBlock::generateISS() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, UINT32_MAX);

    return distrib(gen);
}

void TransmissionControlBlock::sendTCPSegment(IPv4Header &sIPv4Header, TCPHeader &sTCPHeader) {
    //Todo temp
    char sendbuf[SEND_EMPTY_TCP_SEGMENT_LENGTH];

    sIPv4Header.fillSendBuffer(sendbuf);
    sTCPHeader.fillSendBuffer(sendbuf + IP_HEADER_LENGTH, localConnection);
}
