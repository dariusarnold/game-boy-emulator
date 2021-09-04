#include "mmu.hpp"
#include "literals.hpp"
#include <catch2/catch.hpp>


TEST_CASE("MMU is zero initialized") {
    Mmu m;
    auto address = GENERATE(range(0_u16, std::numeric_limits<uint16_t>::max()));
    CHECK(m.read_byte(address) == 0x00);
}

TEST_CASE("Read/write word from mmu", "[mmu]") {
    Mmu m;
    m.write_word(0x0, 0xABCD);
    CHECK(m.read_word(0x00) == 0xABCD);
}

TEST_CASE("Read/write byte from mmu", "[mmu]") {
    Mmu m;
    m.write_byte(0x00, 0xAB);
    m.write_byte(0x01, 0xCD);
    CHECK(m.read_byte(0x00) == 0xAB);
    CHECK(m.read_byte(0x01) == 0xCD);
}

TEST_CASE("Overwriting bytes", "[mmu]") {
    Mmu m;
    uint16_t address = 0x1234;
    m.write_byte(address, 0x11);
    CHECK(m.read_byte(address) == 0x11);
    m.write_byte(address, 0x22);
    CHECK(m.read_byte(address) == 0x22);
}

TEST_CASE("Overwriting words", "[mmu]") {
    Mmu m;
    uint16_t address = 0x1234;
    m.write_word(address, 0x1122);
    CHECK(m.read_word(address) == 0x1122);
    m.write_word(address, 0x2233);
    CHECK(m.read_word(address) == 0x2233);
}