#include <catch2/catch.hpp>
#include "cpu.hpp"


TEST_CASE("OpCode to bit mapping for CB instructions") {
    auto step = GENERATE(range(0, 8));
    auto i = GENERATE(range(0x00, 0x08));
    CHECK(internal::op_code_to_bit(opcodes::OpCode{static_cast<uint8_t>(step * 8 + i)}) == step);
}

