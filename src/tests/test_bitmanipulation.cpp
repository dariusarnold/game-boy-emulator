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
    auto i = GENERATE(range(0, 8));
    std::vector<uint8_t> expected_results = {0b1111'1110, 0b1111'1101, 0b1111'1011, 0b1111'0111,
                                             0b1110'1111, 0b1101'1111, 0b1011'1111, 0b0111'1111};
    uint8_t x = 0xFF;
    bitmanip::unset(x, i);
    CHECK(x == expected_results[i]);
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

TEST_CASE("Rotate left") {
    CHECK(bitmanip::rotate_left(0x00) == 0x00);
    CHECK(bitmanip::rotate_left(0b0000'0001) == 0b0000'0010);
    CHECK(bitmanip::rotate_left(0b1000'0001) == 0b0000'0010);
    CHECK(bitmanip::rotate_left(0xCE) == 0x9C);
}

TEST_CASE("Rotate left through carry bit without flag") {
    bool flag = false;
    CHECK(bitmanip::rotate_left_carry(0x00, flag) == 0x00);
    CHECK_FALSE(flag);
}

TEST_CASE("Rotate left through carry bit with flag") {
    bool flag = true;
    CHECK(bitmanip::rotate_left_carry(0x00, flag) == 0x01);
    CHECK_FALSE(flag);
}

TEST_CASE("Rotate left through carry bit with flag and with carrying") {
    bool flag = true;
    CHECK(bitmanip::rotate_left_carry(0b1100'1110, flag) == 0b1001'1101);
    CHECK(flag);
}

TEST_CASE("Rotate left through carry bit without flag and with carrying") {
    bool flag = false;
    CHECK(bitmanip::rotate_left_carry(0b1100'1110, flag) == 0b1001'1100);
    CHECK(flag);
}

TEST_CASE("Create word from two bytes") {
    CHECK(bitmanip::word_from_bytes(0x12, 0x34) == 0x1234);
}

TEST_CASE("All bits set") {
    uint8_t x = 0xFF;
    auto i = GENERATE(range(0, 8));
    CHECK(bitmanip::bit_value(x, i) == 1);
}

TEST_CASE("No bits set") {
    uint8_t x = 0x0;
    auto i = GENERATE(range(0, 8));
    CHECK(bitmanip::bit_value(x, i) == 0);
}

TEST_CASE("Accessing bits") {
    uint8_t x = 0b10101010;
    CHECK(bitmanip::bit_value(x, 0) == 0);
    CHECK(bitmanip::bit_value(x, 7) == 1);
}