//
// Created by o.narvatov on 11/24/2024.
//

#ifndef TRANSMISSIONCONTROLBLOCK_H
#define TRANSMISSIONCONTROLBLOCK_H
#include <thread>
#include <winsock2.h>

#include "LocalConnection.h"
#include "State.h"
#include "header/IPv4Header.h"
#include "header/TCPHeader.h"


class TransmissionControlBlock {

    // Send Sequence Variables
    //
    //   SND.UNA - send unacknowledged
    //   SND.NXT - send next
    //   SND.WND - send window
    //   SND.UP  - send urgent pointer
    //   SND.WL1 - segment sequence number used for last window update
    //   SND.WL2 - segment acknowledgment number used for last window update
    //   ISS     - initial send sequence number
    //
    // Receive Sequence Variables
    //
    //   RCV.NXT - receive next
    //   RCV.WND - receive window
    //   RCV.UP  - receive urgent pointer
    //   IRS     - initial receive sequence number
    //
    // Current Segment Variables
    //
    //   SEG.SEQ - segment sequence number
    //   SEG.ACK - segment acknowledgment number
    //   SEG.LEN - segment length
    //   SEG.WND - segment window
    //   SEG.UP  - segment urgent pointer
    //   SEG.PRC - segment precedence value

    uint32_t snd_una = 0;
    uint32_t snd_nxt = 0;
    uint32_t iss = 0;

    uint32_t rcv_nxt = 0;
    uint32_t irs = 0;


    std::thread thread;

    void processSegment(const IPv4Header &receiveIPv4Header, const TCPHeader &receiveTCPHeader);

    void run();

    static uint32_t generateISS();

    void sendTCPSegment(IPv4Header &sIPv4Header, TCPHeader &sTCPHeader /*add data buffer and it's length later*/);
public:
    LocalConnection *localConnection;
    bool passive;
    const unsigned timeout;
    SOCKET socket;
    State state = CLOSED;

    TransmissionControlBlock(
        LocalConnection *_localConnection,
        const bool _passive,
        const unsigned _timeout,
        const SOCKET _socket
    ) : localConnection(_localConnection),
        passive(_passive),
        timeout(_timeout),
        socket(_socket)
        {}

    ~TransmissionControlBlock() {
        delete localConnection;
    }

    void start();

};



#endif //TRANSMISSIONCONTROLBLOCK_H
