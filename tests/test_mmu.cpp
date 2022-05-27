#include "mmu.hpp"
#include "literals.hpp"
#include <catch2/catch.hpp>


TEST_CASE("Read/write word from mmu", "[mmu]") {
    Mmu m;
    m.write_word(0xC000, 0xABCD);
    CHECK(m.read_word(0xC000) == 0xABCD);
}

TEST_CASE("Read/write byte from mmu", "[mmu]") {
    Mmu m;
    m.write_byte(0xC000, 0xAB);
    m.write_byte(0xC001, 0xCD);
    CHECK(m.read_byte(0xC000) == 0xAB);
    CHECK(m.read_byte(0xC001) == 0xCD);
}

TEST_CASE("Overwriting bytes", "[mmu]") {
    Mmu m;
    uint16_t address = 0xC000;
    m.write_byte(address, 0x11);
    CHECK(m.read_byte(address) == 0x11);
    m.write_byte(address, 0x22);
    CHECK(m.read_byte(address) == 0x22);
}

TEST_CASE("Overwriting words", "[mmu]") {
    Mmu m;
    uint16_t address = 0xC000;
    m.write_word(address, 0x1122);
    CHECK(m.read_word(address) == 0x1122);
    m.write_word(address, 0x2233);
    CHECK(m.read_word(address) == 0x2233);
}