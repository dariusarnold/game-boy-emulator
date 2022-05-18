#include "catch2/catch.hpp"

#include "instructions/copyregister.hpp"

using namespace registers;

class CopyRegisterFixture {
protected:
    uint8_t source_value = 0;
    uint8_t destination_value = 0;
    Register<A> source_register{source_value};
    Register<A> sd_register{source_value};
    Register<D> destination_register{destination_value};

    void set_source_value(uint8_t val) {
        source_value = val;
    }
};

TEST_CASE_METHOD(CopyRegisterFixture, "Copy one register to different register") {
    set_source_value(0x99);
    CopyRegister cr{source_register, destination_register};
    auto cycles = cr.execute();
    CHECK(cycles == 8);
    CHECK(source_value == 0x99);
    CHECK(destination_value == 0x99);
}

TEST_CASE_METHOD(CopyRegisterFixture, "Copy one register to same register") {
    set_source_value(0x42);
    CopyRegister cr{source_register, sd_register};
    auto cycles = cr.execute();
    CHECK(cycles == 8);
    CHECK(source_value == 0x42);
}
