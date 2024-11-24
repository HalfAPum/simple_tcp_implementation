//
// Created by o.narvatov on 11/24/2024.
//

#ifndef STATE_H
#define STATE_H

enum State {
    LISTEN,
    SYN_SENT,
    SYN_RECEIVED,
    ESTABLISHED,
    CLOSE_WAIT,
    FIN_WAITING_1,
    FIN_WAITING_2,
    CLOSING,
    LAST_ACK,
    TIME_WAIT,
    CLOSED,
};


#endif //STATE_H
