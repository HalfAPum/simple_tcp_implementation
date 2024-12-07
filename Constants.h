//
// Created by o.narvatov on 12/6/2024.
//

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

constexpr auto LISTEN_PORT = "LISTEN_PORT";
constexpr auto PASSIVE_CONNECTION = "PASSIVE_CONNECTION";
constexpr auto CONNECTION_FOREIGN_PORT = "CONNECTION_FOREIGN_PORT";

//Don't include env for address because test now only locally.
constexpr auto ADDR_TO_BIND = "127.0.0.1";

constexpr auto EPHEMERAL_PORT = 0;

constexpr unsigned BUFFLEN = 65535;
constexpr auto RECV_ERROR = -1;

#endif //ENVIRONMENT_H
