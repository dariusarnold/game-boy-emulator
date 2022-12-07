#include <catch2/catch.hpp>
#include "cpu.hpp"


TEST_CASE("OpCode to bit mapping for CB instructions") {
    auto step = GENERATE(range(0, 8));
    auto i = GENERATE(range(0x00, 0x08));
    CHECK(internal::op_code_to_bit(static_cast<uint8_t>(step * 8 + i)) == step);
}

TEST_CASE("Half carry helper function") {
    CHECK(internal::was_half_carry(62, 34, std::plus<>()));
    CHECK_FALSE(internal::was_half_carry(0b01010000, 0b10100000, std::plus<>()));
    CHECK(internal::was_half_carry(16, 52, std::minus<>()));
    CHECK(internal::was_half_carry(0x10, 1, [](int a, int b) { return a - b; }));
    CHECK_FALSE(internal::was_half_carry(0x00, 0x7F, std::plus{}));
}

TEST_CASE("Carry helper function") {
    CHECK(internal::was_carry(255, 1, std::plus<>()));
    CHECK_FALSE(internal::was_carry(254, 1, std::plus<>()));
}

TEST_CASE("Carry word helper function") {
    CHECK(internal::was_carry_word(0x01, 0xFFFF));
    CHECK_FALSE(internal::was_carry_word(0x01, 0xFFFE));
}

TEST_CASE("Half carry word helper function") {
    CHECK(internal::was_half_carry_word(0x1, 0x7FFF));
    CHECK_FALSE(internal::was_half_carry_word(0x1, 0x7FFE));
}