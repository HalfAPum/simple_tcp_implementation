//
// Created by o.narvatov on 11/24/2024.
//

#include "TCPHeader.h"

#include <iostream>

#include "../../byteop/extract/ByteExtractor.h"
#include "../../byteop/insert/ByteInserter.h"

TCPHeader TCPHeader::parseTCPHeader(const unsigned char* recvbuf) {
    TCPHeader header {};

    std::cout << "Parse TCP Header" << std::endl;

    //Source Port (16 bits)
    header.sourcePort = ByteExtractor::get16BitInt(recvbuf + 0);
    std::cout << "Source port: " << static_cast<int>(header.sourcePort) << std::endl;

    //Destination Port (16 bits)
    header.destinationPort = ByteExtractor::get16BitInt(recvbuf + 2);
    std::cout << "Destination port: " << static_cast<int>(header.destinationPort) << std::endl;

    //Sequence Number (32 bits)
    header.sequenceNumber = ByteExtractor::get32BitInt(recvbuf + 4);
    std::cout << "Sequence number: " << header.sequenceNumber << std::endl;

    //Acknowledgment Number (32 bits)
    header.ackNumber = ByteExtractor::get32BitInt(recvbuf + 8);
    std::cout << "Acknowledgment number: " << header.ackNumber << std::endl;

    //Data Offset (4 bits).
    //Data offset is represented in window (window = 1/4 of byte).
    header.dataOffset = ByteExtractor::get4BitInt(recvbuf + 12, true) * 4;
    std::cout << "Data offset: " << static_cast<int>(header.dataOffset) << std::endl;

    //Reserved (6 bits)
    //Get 4 bits left from [recvbuf + 12] and concatenate them with 2 bits from next byte.
    header.reserved = (ByteExtractor::get4BitInt(recvbuf + 12, false) << 2)
            | recvbuf[13] >> 6;
    std::cout << "Reserved: " << static_cast<int>(header.reserved) << std::endl;

    //Flags (6 bits)
    const unsigned char* flagsByte = recvbuf + 13;
    //Urgent Pointer field significant
    header.URG = ByteExtractor::getBit(flagsByte, 2);
    std::cout << "URG: " << header.URG << std::endl;
    //Acknowledgment field significant
    header.ACK = ByteExtractor::getBit(flagsByte, 3);
    std::cout << "ACK: " << header.ACK << std::endl;
    //Push Function
    header.PSH = ByteExtractor::getBit(flagsByte, 4);
    std::cout << "PSH: " << header.PSH << std::endl;
    //Reset the connection
    header.RST = ByteExtractor::getBit(flagsByte, 5);
    std::cout << "RST: " << header.RST << std::endl;
    //Synchronize sequence numbers
    header.SYN = ByteExtractor::getBit(flagsByte, 6);
    std::cout << "SYN: " << header.SYN << std::endl;
    //No more data from sender
    header.FIN = ByteExtractor::getBit(flagsByte, 7);
    std::cout << "FIN: " << header.FIN << std::endl;

    //Window Size (16 bits)
    header.windowSize = ByteExtractor::get16BitInt(recvbuf + 14);
    std::cout << "Window size: " << static_cast<int>(header.windowSize) << std::endl;

    //Checksum (16 bits)
    header.checksum = ByteExtractor::get16BitInt(recvbuf + 16);
    std::cout << "Checksum: " << static_cast<int>(header.checksum) << std::endl;

    //Urgent Pointer (16 bits)
    header.urgentPointer = ByteExtractor::get16BitInt(recvbuf + 18);
    std::cout << "Urgent pointer " << static_cast<int>(header.urgentPointer) << std::endl;

    //Options (variable length)
    //Iterate over options until data starts
    //Default initialize Max Segment Size to max value
    header.maxSegmentSizeOption = 0xFFFF;

    std::cout << "Options: ";
    for (int optionIndex = 20; optionIndex < header.dataOffset;) {
        const uint8_t kind = recvbuf[optionIndex];

        //End of Options List
        if (kind == END_OF_OPTION_LIST_OPTION_KIND) {
            std::cout << "End of Options.";
            break;
        }

        //No-Operation
        if (kind == NO_OPTION_KIND) {
            std::cout << "No-Operation, ";
            ++optionIndex;
            continue;
        }

        //Maximum Segment Size
        if (kind == MAX_SEGMENT_SIZE_OPTION_KIND) {
            //To ignore kind and length add 2 to option index
            header.maxSegmentSizeOption = ByteExtractor::get16BitInt(recvbuf + (optionIndex + 2));
            std::cout << "Maximum Segment Size: " + std::to_string(header.maxSegmentSizeOption) + ", ";
            optionIndex += 4;
            continue;
        }

        //Unknown segment
        break;
    }

    std::cout << std::endl;

    std::cout << std::endl;

    return header;
}

TCPHeader TCPHeader::constructSendTCPHeader(const uint16_t localPort) const {
    TCPHeader sTCPHeader {};

    sTCPHeader.sourcePort = localPort;
    sTCPHeader.destinationPort = sourcePort;

    //No SEQ number for empty header.
    sTCPHeader.sequenceNumber = 0;
    //No ACK Number for empty header.
    sTCPHeader.ackNumber = 0;

    sTCPHeader.reserved = 0;
    sTCPHeader.URG = false;
    sTCPHeader.ACK = false;
    sTCPHeader.PSH = false;
    sTCPHeader.RST = false;
    sTCPHeader.SYN = false;
    sTCPHeader.FIN = false;

    //Set max window size we are ready to accept.
    sTCPHeader.windowSize = 0xFFFF;

    //Calculate checksum before sending segment.
    // uint16_t checksum;

    //No URG pointer for empty header.
    sTCPHeader.urgentPointer = 0;

    //Set Max Segment Size.
    sTCPHeader.maxSegmentSizeOption = 0xFFFF;

    //Data offset represents offset in "word" one word is 1/4 of byte.
    sTCPHeader.dataOffset = SEND_TCP_HEADER_LENGTH / 4;

    return sTCPHeader;
}

void TCPHeader::fillSendBuffer(unsigned char*sendbuff) const {
    ByteInserter::insert16BitInt(sendbuff, sourcePort);
    ByteInserter::insert16BitInt(sendbuff + 2, destinationPort);
    ByteInserter::insert32BitInt(sendbuff + 4, sequenceNumber);
    ByteInserter::insert32BitInt(sendbuff + 8, ackNumber);
    ByteInserter::insert8BitInt(sendbuff + 12, dataOffset << 4 | reserved >> 2);
    const uint8_t flags = URG << 5 | ACK << 4 | PSH << 3 | RST << 2 | SYN << 1 | FIN;
    ByteInserter::insert8BitInt(sendbuff + 13, reserved << 6 | flags);
    ByteInserter::insert16BitInt(sendbuff + 14, windowSize);
    //Insert 0 checksum. After headers and text are filled, calculate checksum and insert it.
    ByteInserter::insert16BitInt(sendbuff + 16, 0);
    ByteInserter::insert16BitInt(sendbuff + 18, urgentPointer);

    /*
     * Data offset is equal to TCP header size + options.
     * Options we send are Maximum Segment Size, No-Option, End of Option List.
     * TCP header must occupy bytes count multiples of 4 e.g. 20, 24, 28...
     * Max Segment Size Option occupy 4 bytes.
     * Options must end with End of Option List Option which occupies 1 byte.
     * Add 3 additional No-Option Options (1 byte each) to make TCP header fit bytes count multiples of 4 rule.
     * Total byte count is 28 bytes.
     *
     */
    ByteInserter::insert8BitInt(sendbuff + 20, MAX_SEGMENT_SIZE_OPTION_KIND);
    ByteInserter::insert8BitInt(sendbuff + 21, MAX_SEGMENT_SIZE_OPTION_LENGTH);
    ByteInserter::insert16BitInt(sendbuff + 22, maxSegmentSizeOption);
    ByteInserter::insert8BitInt(sendbuff + 24, NO_OPTION_KIND);
    ByteInserter::insert8BitInt(sendbuff + 25, NO_OPTION_KIND);
    ByteInserter::insert8BitInt(sendbuff + 26, NO_OPTION_KIND);
    ByteInserter::insert8BitInt(sendbuff + 27, END_OF_OPTION_LIST_OPTION_KIND);
}

void TCPHeader::calculateChecksum(const IPv4Header &ipv4Header, unsigned char* sendbuf) {
    uint32_t sum = 0;

    //Add pseudo header first.
    sum += ipv4Header.sourceIPAddress >> 16 & 0xFFFF;
    sum += ipv4Header.sourceIPAddress & 0xFFFF;
    sum += ipv4Header.destinationIPAddress >> 16 & 0xFFFF;
    sum += ipv4Header.destinationIPAddress & 0xFFFF;
    sum += IPv4_TCP_PROTOCOL & 0xFFFF;
    //TODO UPDATE WHEN SEND DATA
    sum += SEND_TCP_HEADER_LENGTH & 0xFFFF;

    //Add TCP header
    //TODO consider for odd end.
    //TODO add data to checksum
    for (int i = 0; i < SEND_TCP_HEADER_LENGTH; i+=2) {
        if (i+1 == SEND_TCP_HEADER_LENGTH) std::cout << "FUCK THIS IS A PROBLEM" << std::endl;
        //TODO maybe & 0xFFFF is redundant?
        sum += (sendbuf[i] << 8 | sendbuf[i + 1]) & 0xFFFF;
    }

    //Fold 32-bit sum to 16 bits.
    sum = (sum & 0xFFFF) + (sum >> 16 & 0xFFFF);

    //Perform complement.
    sum = ~sum;

    checksum = sum;
    ByteInserter::insert16BitInt(sendbuf + 16, sum);
}
