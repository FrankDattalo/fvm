//
// Created by Frank Dattalo on 6/3/18.
//

#define CATCH_CONFIG_MAIN
#include "cmake-build-debug/catch.hpp"

#include <string>

TEST_CASE("Catch Hello World", "[hello-world]") {

    std::string expected = "Hello, World!";

    REQUIRE(expected == "Hello, World!");
}

TEST_CASE("Virtual Machine Performs Additions", "[virtual-machine]") {

}