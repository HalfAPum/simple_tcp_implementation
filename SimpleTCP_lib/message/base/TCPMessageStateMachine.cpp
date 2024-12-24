//
// Created by o.narvatov on 12/20/2024.
//

#include "TCPMessageStateMachine.h"

TCPMessageStateMachine* TCPMessageStateMachine::singleton = nullptr;

TCPMessageStateMachine *TCPMessageStateMachine::initialize(TCPMessageStateMachine *other) {
    singleton = other;

    return singleton;
}