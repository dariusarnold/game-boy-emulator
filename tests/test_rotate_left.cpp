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
    Register<A> register_{a};
};

TEST_CASE_METHOD(RotateLeftFixture, "Test rotate left instruction") {
    a = 0x01;
    RotateLeft rl{zero_flag, subtract_flag, half_carry_flag, carry_flag, register_};
    rl.execute();
    REQUIRE(a == 0x02);
    rl.execute();
    REQUIRE(a == 0x04);
    rl.execute();
    REQUIRE(a == 0x08);
    rl.execute();
    REQUIRE(a == 0x10);
    rl.execute();
    REQUIRE(a == 0x20);
    rl.execute();
    REQUIRE(a == 0x40);
    rl.execute();
    REQUIRE(a == 0x80);
}

TEST_CASE_METHOD(RotateLeftFixture, "Test rotate left instruction setting zero flag") {
    a = 0x00;
    RotateLeft rl{zero_flag, subtract_flag, half_carry_flag, carry_flag, register_};
    rl.execute();
    REQUIRE(a == 0x00);
    CHECK(zero_flag.read());
    zero_flag.set_inactive();
    a = 0x80;
    rl.execute();
    REQUIRE(a == 0x00);
    CHECK(zero_flag.read());
}

TEST_CASE_METHOD(RotateLeftFixture, "Test rotate left instruction resetting zero flag") {
    a = 0x01;
    zero_flag.set_active();
    RotateLeft rl{zero_flag, subtract_flag, half_carry_flag, carry_flag, register_};
    rl.execute();
    REQUIRE(a == 0x02);
    CHECK_FALSE(zero_flag.read());
}

TEST_CASE_METHOD(RotateLeftFixture, "Test rotate left instruction resetting subtraction flag") {
    a = 0x01;
    subtract_flag.set_active();
    RotateLeft rl{zero_flag, subtract_flag, half_carry_flag, carry_flag, register_};
    rl.execute();
    CHECK_FALSE(subtract_flag.read());
}

TEST_CASE_METHOD(RotateLeftFixture, "Test rotate left instruction resetting half carry flag") {
    a = 0x08;
    half_carry_flag.set_active();
    RotateLeft rl{zero_flag, subtract_flag, half_carry_flag, carry_flag, register_};
    rl.execute();
    CHECK_FALSE(half_carry_flag.read());
}

TEST_CASE_METHOD(RotateLeftFixture, "Test rotate left instruction setting carry flag") {
    a = 0x80;
    RotateLeft rl{zero_flag, subtract_flag, half_carry_flag, carry_flag, register_};
    rl.execute();
    CHECK(carry_flag.read());
}

TEST_CASE_METHOD(RotateLeftFixture, "Test rotate left instruction resetting carry flag") {
    a = 0x0F;
    RotateLeft rl{zero_flag, subtract_flag, half_carry_flag, carry_flag, register_};
    rl.execute();
    CHECK_FALSE(carry_flag.read());
}

TEST_CASE_METHOD(RotateLeftFixture, "Test rotate left instruction with carry through carry flag") {
    a = 0x80;
    RotateLeft rl{zero_flag, subtract_flag, half_carry_flag, carry_flag, register_, true};
    rl.execute();
    CHECK(a == 0x00);
    CHECK(carry_flag.read());
}

TEST_CASE_METHOD(RotateLeftFixture, "Test rotate left instruction with carry flag set") {
    a = 0x80;
    carry_flag.set_active();
    RotateLeft rl{zero_flag, subtract_flag, half_carry_flag, carry_flag, register_, true};
    rl.execute();
    CHECK(a == 0x01);
    CHECK(carry_flag.read());
}

TEST_CASE_METHOD(RotateLeftFixture, "Test rotate left instruction with carry flag set2") {
    a = 0xCE;
    carry_flag.set_inactive();
    RotateLeft rl{zero_flag, subtract_flag, half_carry_flag, carry_flag, register_, true};
    rl.execute();
    CHECK(a == 0x9C);
    CHECK(carry_flag.read());
}

TEST_CASE_METHOD(RotateLeftFixture, "Test rotate left without carry flag set") {
    a = 0xCE;
    carry_flag.set_active();
    RotateLeft rl{zero_flag, subtract_flag, half_carry_flag, carry_flag, register_, true};
    rl.execute();
    CHECK(a == 0x9D);
    CHECK(carry_flag.read());
}