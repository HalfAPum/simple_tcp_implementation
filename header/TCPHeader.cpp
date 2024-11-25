//
// Created by o.narvatov on 11/24/2024.
//

#include "TCPHeader.h"

#include <iostream>

#include "../ByteExtractor.h"

TCPHeader TCPHeader::parseTCPHeader(const char* recvbuf) {
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

    //Data Offset (4 bits)
    header.dataOffset = ByteExtractor::get4BitInt(recvbuf + 12, true);
    std::cout << "Data offset: " << static_cast<int>(header.dataOffset) << std::endl;

    //Reserved (6 bits)
    //Get 4 bits left from [recvbuf + 12] and concatenate them with 2 bits from next byte.
    header.reserved = (ByteExtractor::get4BitInt(recvbuf + 12, false) << 2)
            | recvbuf[13] >> 6;
    std::cout << "Reserved: " << static_cast<int>(header.reserved) << std::endl;

    //Flags (6 bits)
    const char* flagsByte = recvbuf + 13;
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
        if (kind == 0x00) {
            std::cout << "End of Options.";
            break;
        }

        //No-Operation
        if (kind == 0x01) {
            std::cout << "No-Operation, ";
            ++optionIndex;
            continue;
        }

        //Maximum Segment Size
        if (kind == 0x02) {
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

void TCPHeader::fillSendBuffer(char *sendbuff, LocalConnection *localConnection) {

}
