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
}
