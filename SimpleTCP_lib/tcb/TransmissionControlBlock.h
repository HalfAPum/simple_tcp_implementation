//
// Created by o.narvatov on 11/24/2024.
//

#ifndef TRANSMISSIONCONTROLBLOCK_H
#define TRANSMISSIONCONTROLBLOCK_H
#include <winsock2.h>

#include "LocalConnection.h"
#include "State.h"
#include "../header/ipv4/IPv4Header.h"
#include "../header/tcp/TCPHeader.h"
#include "../header/udp/UDPHeader.h"


class TransmissionControlBlock {
    void sendTCPSegment(TCPHeader &sTCPHeader /*add data buffer and it's length later*/);

    bool threadLaunched = false;

    void launchTCBThreadInternal();
public:
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

    LocalConnection *localConnection;
    bool passive;
    const unsigned timeout;
    SOCKET connectionSocket;
    State state = CLOSED;

    TransmissionControlBlock(
        LocalConnection *_localConnection,
        const bool _passive,
        const unsigned _timeout
    ) : localConnection(_localConnection),
        passive(_passive),
        timeout(_timeout),
        connectionSocket(INVALID_SOCKET),
        state(passive ? LISTEN : CLOSED) {}

    ~TransmissionControlBlock() {
        delete localConnection;
    }

    void processListeningSocketMessage(const TCPHeader & tcpHeader);

    void sendSYN(uint16_t foreignPort);

    void launchTCBThread();

    static uint32_t generateISS();

    void processPacketListenState(const TCPHeader &tcpHeader);
};



#endif //TRANSMISSIONCONTROLBLOCK_H
