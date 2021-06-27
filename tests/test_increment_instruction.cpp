#include "catch2/catch.hpp"
#include "instructions/increment.hpp"

using namespace registers;

class IncrementTestFixture {
protected:
    uint8_t a = 0;
    uint16_t hl = 0;
    MutableRegister<A> a_register{a};
    MutableRegister<HL> hl_register{hl};
    uint8_t f = 0;
    MutableFlag<flags::half_carry> hc_flag{f};
    MutableFlag<flags::zero> zero_flag{f};
    MutableFlag<flags::subtract> subtract_flag{f};
    uint16_t pc = 0;
    ProgramCounterIncDec increment_pc{MutableRegister<PC>{pc}};
};

TEST_CASE_METHOD(IncrementTestFixture, "Test increment 1 byte register increments") {
    Increment<A> increment{a_register, zero_flag, subtract_flag, hc_flag, increment_pc};
    auto cycles = increment.execute();
    CHECK(cycles == 4);
    CHECK(a == 1);
    CHECK(pc == 1);
    CHECK(hc_flag.read() == false);
    CHECK(zero_flag.read() == false);
    CHECK(subtract_flag.read() == false);
}

TEST_CASE_METHOD(IncrementTestFixture, "Test increment 1 byte register half carry flag") {
    a = 0b00001111;
    Increment<A> increment{a_register, zero_flag, subtract_flag, hc_flag, increment_pc};
    auto cycles = increment.execute();
    CHECK(cycles == 4);
    CHECK(a == 0b00001111 + 1);
    CHECK(pc == 1);
    CHECK(hc_flag.read() == true);
    CHECK(zero_flag.read() == false);
    CHECK(subtract_flag.read() == false);
}

TEST_CASE_METHOD(IncrementTestFixture, "Test increment 1 byte register zero flag") {
    a = 0xFF;
    Increment<A> increment{a_register, zero_flag, subtract_flag, hc_flag, increment_pc};
    auto cycles = increment.execute();
    CHECK(cycles == 4);
    CHECK(a == 0);
    CHECK(pc == 1);
    CHECK(hc_flag.read() == true);
    CHECK(zero_flag.read() == true);
    CHECK(subtract_flag.read() == false);
}

TEST_CASE_METHOD(IncrementTestFixture, "Test increment 2 byte register") {
    Increment<HL> increment{hl_register, zero_flag, subtract_flag, hc_flag, increment_pc};
    auto cycles = increment.execute();
    CHECK(cycles == 8);
    CHECK(hl == 1);
    CHECK(pc == 1);
    CHECK(hc_flag.read() == false);
    CHECK(zero_flag.read() == false);
    CHECK(subtract_flag.read() == false);
}

TEST_CASE_METHOD(IncrementTestFixture, "Test increment 2 byte register half carry flag") {
    hl = 0b0000'1111'1111'1111;
    Increment<HL> increment{hl_register, zero_flag, subtract_flag, hc_flag, increment_pc};
    auto cycles = increment.execute();
    CHECK(cycles == 8);
    CHECK(hl == 0b0000'1111'1111'1111 + 1);
    CHECK(pc == 1);
    CHECK(hc_flag.read() == true);
    CHECK(zero_flag.read() == false);
    CHECK(subtract_flag.read() == false);
}

TEST_CASE_METHOD(IncrementTestFixture, "Test increment 2 byte register zero flag") {
    hl = 0xFFFF;
    Increment<HL> increment{hl_register, zero_flag, subtract_flag, hc_flag, increment_pc};
    auto cycles = increment.execute();
    CHECK(cycles == 8);
    CHECK(hl == 0);
    CHECK(pc == 1);
    CHECK(hc_flag.read() == true);
    CHECK(zero_flag.read() == true);
    CHECK(subtract_flag.read() == false);
}