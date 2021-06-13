#include "catch2/catch.hpp"

#include "instructions/copyregister.hpp"

class CopyRegisterFixture {
protected:
    uint8_t source_value = 0;
    uint8_t destination_value = 0;
    Register<A> source_register{source_value};
    MutableRegister<A> sd_register{source_value};
    MutableRegister<D> destination_register{destination_value};
    uint16_t pc = 0;
    IncrementPC increment_pc{MutableRegister<PC>{pc}};

    void set_source_value(uint8_t val) {
        // Since Register stores the value it tracks as a copy, it doesnt reflect changes after
        // creating register. Because of this overwrite source_register.
        source_value = val;
        source_register = Register<A>{source_value};
    }
};

TEST_CASE_METHOD(CopyRegisterFixture, "Copy one register to different register") {
    set_source_value(0x99);
    CopyRegister cr{source_register, destination_register, increment_pc};
    auto cycles = cr.execute();
    CHECK(cycles == 8);
    CHECK(source_value == 0x99);
    CHECK(destination_value == 0x99);
    CHECK(pc == 0x0001);
}

TEST_CASE_METHOD(CopyRegisterFixture, "Copy one register to same register") {
    set_source_value(0x42);
    CopyRegister cr{source_register, sd_register, increment_pc};
    auto cycles = cr.execute();
    CHECK(cycles == 8);
    CHECK(source_value == 0x42);
    CHECK(pc == 0x0001);
}
