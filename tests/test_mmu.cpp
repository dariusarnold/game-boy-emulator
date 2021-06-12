#include "mmu.hpp"
#include "literals.hpp"
#include <catch2/catch.hpp>


TEST_CASE("MMU is zero initialized") {
    Mmu m;
    auto address = GENERATE(range(0_u16, std::numeric_limits<uint16_t>::max()));
    CHECK(m.read_memory(address) == 0x00);
}

TEST_CASE("Reading word from mmu") {
    Mmu m;
    m.write_memory(0x00, 0xAB);
    m.write_memory(0x01, 0xCD);
    CHECK(m.read_memory_word(0x00) == 0xCDAB);
}