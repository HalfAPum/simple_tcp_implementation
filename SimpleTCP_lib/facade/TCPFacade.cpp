//
// Created by o.narvatov on 12/16/2024.
//

#include "TCPFacade.h"


TCPFacade* TCPFacade::singleton = nullptr;

TCPFacade *TCPFacade::initialize(TCPFacade *other) {
    if (singleton == nullptr){
        singleton = other;
    }

    return singleton;
}