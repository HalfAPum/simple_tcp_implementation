//
// Created by o.narvatov on 12/10/2024.
//
#include <catch2/catch_all.hpp>
#include <catch2/internal/catch_preprocessor_internal_stringify.hpp>

#include "SimpleTCP.h"

TEST_CASE("TCPHandshakeTest", "Passive") {
    SimpleTCP simpleTcp {};

    bool initialized = simpleTcp.initialize();

    REQUIRE(initialized);

    auto localConnection = simpleTcp.open();


    simpleTcp.close()
}