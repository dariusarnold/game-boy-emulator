#include "catch2/catch.hpp"
#include "instructions/rotateleft.hpp"

using namespace registers;

class RotateLeftFixture {
protected:
    uint8_t f = 0;
    MutableFlag<flags::zero> zero_flag{f};
    MutableFlag<flags::subtract> subtract_flag{f};
    MutableFlag<flags::half_carry> half_carry_flag{f};
    MutableFlag<flags::carry> carry_flag{f};
    uint8_t a;
    MutableRegister<A> register_{a};
};

TEST_CASE_METHOD(RotateLeftFixture, "Test rotate left instruction") {
    // TODO Add tests
}