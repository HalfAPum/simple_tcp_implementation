//
// Created by o.narvatov on 12/10/2024.
//
#include <catch2/catch_all.hpp>

#include "SimpleTCP.h"

TEST_CASE("ExampleDate", "[TCPHandshakeTest]") {
    CHECK(true);

    SimpleTCP simpleTcp {};

    CHECK(simpleTcp.initialize());
}