#include "cpu.hpp"
#include <catch2/catch.hpp>
#include "iostream"


TEST_CASE("Registers are zero initialized") {
    Registers r;
    CHECK(r.af == 0);
    CHECK(r.sp == 0);
    CHECK(r.pc == 0);
    CHECK(r.bc == 0);
    CHECK(r.de == 0);
    CHECK(r.hl == 0);
}

TEST_CASE("Accessing low register byte after writing to combined register") {
    Registers r{};
    r.af = 1;
    CHECK(r.a == 0);
    CHECK(r.f == 1);
    r.af = 2;
    CHECK(r.a == 0);
    CHECK(r.f == 2);
}

TEST_CASE("Accessing high register byte after writing to combined register") {
    Registers r{};
    r.bc = 0b100000000;
    CHECK(r.b == 1);
    CHECK(r.c == 0);
}

TEST_CASE("Accessing combined register after writing to single byte registers") {
    Registers r{};
    r.hl = 0b0000000100000001;
    CHECK(r.h == 1);
    CHECK(r.l == 1);
}

TEST_CASE("Writing to the value should update all registers for that value") {
    uint8_t a = 0;
    Register<registers::A> r1{a};
    a = 1;
    CHECK(r1.get() == 1);
}

TEST_CASE("Writing through Register should update all registers for that value") {
    uint8_t a = 0;
    Register<registers::A> r1{a};
    Register<registers::A> r2{a};
    r2.set(1);
    CHECK(r1.get() == 1);
}