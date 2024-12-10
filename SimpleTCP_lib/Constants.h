//
// Created by o.narvatov on 12/6/2024.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

//Environment variables
constexpr auto LISTEN_PORT = "LISTEN_PORT";
constexpr auto PASSIVE_CONNECTION = "PASSIVE_CONNECTION";
constexpr auto CONNECTION_FOREIGN_PORT = "CONNECTION_FOREIGN_PORT";

//Don't include env for address because test now only locally.
constexpr auto ADDR_TO_BIND = "127.0.0.1";

//Port
constexpr auto EPHEMERAL_PORT = 0;
constexpr auto DEFAULT_TCP_PORT = 80;

//Message
constexpr unsigned BUFFLEN = 65535;
constexpr auto RECV_ERROR = -1;

//Header
constexpr int IP_HEADER_LENGTH = 20;
constexpr int UDP_HEADER_LENGTH = 8;
constexpr int TCP_HEADER_MIN_LENGTH = 20;
constexpr int TCP_SEGMENT_MIN_LENGTH = IP_HEADER_LENGTH + UDP_HEADER_LENGTH + TCP_HEADER_MIN_LENGTH;

#endif //CONSTANTS_H
