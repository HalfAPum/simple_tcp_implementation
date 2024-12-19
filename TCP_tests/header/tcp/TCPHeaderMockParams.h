//
// Created by o.narvatov on 12/19/2024.
//

#ifndef TCHHEADERMOCKPARAMS_H
#define TCHHEADERMOCKPARAMS_H



struct TCPHeaderMockParams {
    int bufferSize;
    bool ignoreRawSock;

    TCPHeaderMockParams(const int _bufferSize, const bool _ignoreRawSock) : bufferSize(_bufferSize), ignoreRawSock(_ignoreRawSock) {}
};



#endif //TCHHEADERMOCKPARAMS_H
