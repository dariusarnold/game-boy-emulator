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

TEST_CASE("Check if bit is set") {
    uint8_t x = 0xFF;
    auto i = GENERATE(range(0, 8));
    CHECK(bitmanip::is_bit_set(x, i));
}

TEST_CASE("Check if bit is unset") {
    uint8_t x = 0x00;
    auto i = GENERATE(range(0, 8));
    CHECK_FALSE(bitmanip::is_bit_set(x, i));
}

TEST_CASE("Check if bit is set in 2 byte int") {
    uint16_t x = 0b0010'0000'0000'1000;
    CHECK(bitmanip::is_bit_set(x, 3));
    CHECK(bitmanip::is_bit_set(x, 13));
    CHECK_FALSE(bitmanip::is_bit_set(x, 0));
}

TEST_CASE("Test getting low byte from word") {
    CHECK(bitmanip::get_low_byte(0xFF00) == 0x00);
    CHECK(bitmanip::get_low_byte(0x1234) == 0x34);
}

TEST_CASE("Test getting high byte from word") {
    CHECK(bitmanip::get_high_byte(0xFF00) == 0xFF);
    CHECK(bitmanip::get_high_byte(0x1234) == 0x12);
}

TEST_CASE("Get low nibble from byte") {
    CHECK(bitmanip::get_low_nibble(0x34) == 0x04);
    CHECK(bitmanip::get_low_nibble(0x1A) == 0x0A);
    CHECK(bitmanip::get_low_nibble(0x20) == 0x00);
}

TEST_CASE("Get high nibble from byte") {
    CHECK(bitmanip::get_high_nibble(0x34) == 0x03);
    CHECK(bitmanip::get_high_nibble(0x1A) == 0x01);
    CHECK(bitmanip::get_high_nibble(0x20) == 0x02);
}