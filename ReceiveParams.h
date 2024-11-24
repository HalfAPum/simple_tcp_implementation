//
// Created by o.narvatov on 11/24/2024.
//

#ifndef RECEIVEPARAMS_H
#define RECEIVEPARAMS_H


struct ReceiveParams {
    const unsigned byteCount;
    const bool URG;
    const bool PSH;
};


#endif //RECEIVEPARAMS_H
