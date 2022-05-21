#include "catch2/catch.hpp"
#include "instructions/increment.hpp"

using namespace registers;

class IncrementTestFixture {
protected:
    uint8_t a = 0;
    uint16_t hl = 0;
    Register<A> a_register{a};
    Register<HL> hl_register{hl};
    uint8_t f = 0;
    MutableFlag<flags::half_carry> hc_flag{f};
    MutableFlag<flags::zero> zero_flag{f};
    MutableFlag<flags::subtract> subtract_flag{f};
};

TEST_CASE_METHOD(IncrementTestFixture, "Test increment 1 byte register increments") {
    IncrementByte<A> increment{a_register, zero_flag, subtract_flag, hc_flag};
    auto cycles = increment.execute();
    CHECK(cycles == 4);
    CHECK(a == 1);
    CHECK(hc_flag.read() == false);
    CHECK(zero_flag.read() == false);
    CHECK(subtract_flag.read() == false);
}

TEST_CASE_METHOD(IncrementTestFixture, "Test increment 1 byte register half carry flag") {
    a = 0b00001111;
    IncrementByte<A> increment{a_register, zero_flag, subtract_flag, hc_flag};
    auto cycles = increment.execute();
    CHECK(cycles == 4);
    CHECK(a == 0b00001111 + 1);
    CHECK(hc_flag.read() == true);
    CHECK(zero_flag.read() == false);
    CHECK(subtract_flag.read() == false);
}

TEST_CASE_METHOD(IncrementTestFixture, "Test increment 1 byte register setting zero flag") {
    a = 0xFF;
    IncrementByte<A> increment{a_register, zero_flag, subtract_flag, hc_flag};
    auto cycles = increment.execute();
    CHECK(cycles == 4);
    CHECK(a == 0);
    CHECK(hc_flag.read() == true);
    CHECK(zero_flag.read() == true);
    CHECK(subtract_flag.read() == false);
}

TEST_CASE_METHOD(IncrementTestFixture, "Test increment 1 byte register resetting zero flag") {
    a = 0x01;
    zero_flag.set_active();
    IncrementByte<A> increment{a_register, zero_flag, subtract_flag, hc_flag};
    auto cycles = increment.execute();
    CHECK(cycles == 4);
    CHECK(a == 0x02);
    CHECK(hc_flag.read() == false);
    CHECK(zero_flag.read() == false);
    CHECK(subtract_flag.read() == false);
}

TEST_CASE_METHOD(IncrementTestFixture, "Test increment 2 byte") {
    hl = 0b0000'1111'1111'1111;
    IncrementWord<HL> increment{hl_register};
    auto cycles = increment.execute();
    CHECK(cycles == 8);
    CHECK(hl == 0b0000'1111'1111'1111 + 1);
}

TEST_CASE_METHOD(IncrementTestFixture, "Test increment 2 byte register overflow") {
    hl = 0xFFFF;
    IncrementWord<HL> increment{hl_register};
    auto cycles = increment.execute();
    CHECK(cycles == 8);
    CHECK(hl == 0);
}