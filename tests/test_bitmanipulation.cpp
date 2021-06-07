#include "bitmanipulation.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Setting bit value to 1") {
    uint8_t x = 0;
    for (int i = 0; i < 8; ++i) {
        bitmanip::set(x, i);
        CHECK(x == 1U << i);
        x = 0;
    }
}

TEST_CASE("Setting bit value to 0") {
    uint8_t x = 0xFF;
    for (int i = 0; i < 8; ++i) {
        bitmanip::unset(x, i);
        CHECK(x == 0xFF - (1U << i));
        x = 0xFF;
    }
}
