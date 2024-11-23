#include <cstdint>
#include <iomanip>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#define ADDR_TO_BIND "127.0.0.1"
#define DEFAULT_PORT 8080
#define BUFFLEN 65535
#define RECV_ERROR (-1)

constexpr int IP_HEADER_LENGTH = 20;
constexpr int TCP_HEADER_LENGTH = 20;
constexpr int TCP_SEGMENT_LENGTH = IP_HEADER_LENGTH + TCP_HEADER_LENGTH;

bool checkResultFail(const bool result, const std::string &actionName, const SOCKET socket) {
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

int main() {
    WSAData wsaData = {};

    //Initialize winsock
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup faield with error: " << iResult << std::endl;
        return 1;
    }

    //Try IPPROTO_RAW when succeed with UDP
    SOCKET listenSocket = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
    if (listenSocket == INVALID_SOCKET) {
        std::cout << "socket creation faield with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    if (!setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEADDR, nullptr, 0)) {
        perror("setsockopt");
        return 1;
    }

    sockaddr_in sockstr {};
    sockstr.sin_addr.s_addr = inet_addr(ADDR_TO_BIND);
    sockstr.sin_family = AF_INET;
    sockstr.sin_port = DEFAULT_PORT;
    constexpr auto sockstr_size = static_cast<socklen_t>(sizeof(sockstr));

    iResult = bind(listenSocket, reinterpret_cast<sockaddr *>(&sockstr), sockstr_size);
    if (checkResultFail(iResult == SOCKET_ERROR, "bind", listenSocket)) {
        return 1;
    }
    std::cout << "bind result " << iResult << std::endl;

    char recvbuf[BUFFLEN];
    int recvbuflen = BUFFLEN;

    while (true) {
        const int recvResult = recv(listenSocket, recvbuf, recvbuflen, 0);

        if (checkResultFail(recvResult == RECV_ERROR, "recvResult", listenSocket)) {
            return 1;
        }

        //Verify received Result size.
        //We should receive IP and TCP headers (each is 20 bytes long).
        //Data payload is optional.
        if (validate(recvResult < TCP_SEGMENT_LENGTH, listenSocket,
            "Received message has size: " + std::to_string(recvResult) +
            ". This is less than TCP_SEGMENT_LENGTH " + std::to_string(TCP_SEGMENT_LENGTH)
        )) {
            return 1;
        }

        //Each char is a byte (1 byte = 8 bits)

        //Parse IP header

        //Version (4 bits)
        uint8_t byte = recvbuf[0];
        uint8_t version = (byte >> 4) & 0x0F;
        std::cout << "Version: " << (int)version << std::endl;

        //Header length (4 bits)
        uint8_t headerLength = byte & 0x0F;
        std::cout << "Header Length: " << (int)headerLength << std::endl;

        //Type of Service (8 bits)
        uint8_t typeOfService = recvbuf[1];
        std::cout << "Type of Service: " << (int)typeOfService << std::endl;

        //Total Length (16 bits)
        uint16_t totalLength = (recvbuf[2] << 8) | recvbuf[3];
        std::cout << "Total length: " << (int)totalLength << std::endl;

        //Identification (16 bits)
        uint16_t identification = (recvbuf[4] << 8) | recvbuf[5];
        std::cout << "Identification: " << (int)identification << std::endl;

        //Flags (3 bits)
        byte = recvbuf[6];
        //  Reserved (1 bit)
        bool reserved = byte & 0x80;
        std::cout << "Reserved: " << reserved << std::endl;
        //  Don't Fragment (1 bit)
        bool dontFragment = byte & 0x40;
        std::cout << "Don't Fragment: " << dontFragment << std::endl;
        //  More Fragments (1 bit)
        bool moreFragments = byte & 0x20;
        std::cout << "More Fragments: " << moreFragments << std::endl;

        //Fragment Offset (13 bits)
        //Eliminate Flags bits
        byte = byte & 0x1F;
        uint16_t fragmentOffset = (byte << 8) | recvbuf[7];
        std::cout << "Fragment Offset: " << (int)fragmentOffset << std::endl;

        //Time to Live (8 bits)
        uint8_t timeToLive = recvbuf[8];
        std::cout << "Time to Live: " << (int)timeToLive << std::endl;

        //Protocol (8 bits)
        uint8_t protocol = recvbuf[9];
        std::cout << "Protocol: " << (int)protocol << std::endl;

        //Header Checksum (16 bits)
        uint16_t headerChecksum = (recvbuf[10] << 8) | recvbuf[11];
        std::cout << "Header Checksum: " << (int)headerChecksum << std::endl;

        //Source IP Address (32 bits)
        uint32_t sourceIPAddress = (recvbuf[12] << 24) | (recvbuf[13] << 16) | (recvbuf[14] << 8) | recvbuf[15];
        std::cout << "Source IP Address: " << (int)sourceIPAddress << std::endl;

        //Destination IP Address (32 bits)
        uint32_t destinationIPAddress = (recvbuf[16] << 24) | (recvbuf[17] << 16) | (recvbuf[18] << 8) | recvbuf[19];
        std::cout << "Destination IP Address: " << (int)destinationIPAddress << std::endl;


        //Options (variable length)
        //Ignore Options. Pray they are not present)

        std::cout << std::endl;
        printMessage(recvbuf, recvResult);
        closesocket(listenSocket);
        break;
    }

    return 0;
}

// IP Header:
//
// Version (4 bits)
// Header Length (4 bits)
// Type of Service (8 bits)
// Total Length (16 bits)
// Identification (16 bits)
// Flags (3 bits)
// Fragment Offset (13 bits)
// Time to Live (8 bits)  
// Protocol (8 bits)
// Header Checksum (16 bits)
// Source IP Address (32 bits)
// Destination IP Address (32 bits)
// Options (variable length)  
// TCP Header:
//
// Source Port (16 bits)
// Destination Port (16 bits)
// Sequence Number (32 bits)
// Acknowledgment Number (32 bits)
// Data Offset (4 bits)
// Reserved (6 bits)
// Flags (6 bits)  
// Window Size (16 bits)
// Checksum (16 bits)
// Urgent Pointer (16 bits)
// Options (variable length)
// Data (variable length)