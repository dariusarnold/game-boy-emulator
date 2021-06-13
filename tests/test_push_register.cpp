#include "catch2/catch.hpp"
#include "instructions/pushregister.hpp"


class PushRegisterFixture {
protected:
    uint16_t bc = 0x1234;
    Register<BC> register_bc{bc};
    // Stack grows down, so we have to use a value != 0 here to avoid underflow
    uint16_t sp = 0x08;
    MutableRegister<SP> register_sp{sp};
    Mmu mmu;
    MutableMemory memory{mmu};
    MutableStack stack{memory, register_sp};
    uint16_t pc = 0;
    IncrementPC increment_pc{MutableRegister<PC>{pc}};
};


TEST_CASE_METHOD(PushRegisterFixture, "Pushing register to stack") {
    PushRegisterOnStack<BC> push_register_on_stack{register_bc, stack, increment_pc};
    auto cycles = push_register_on_stack.execute();
    CHECK(cycles == 16);
    CHECK(pc == 1);
    CHECK(stack.peek() == bc);
    CHECK(sp == 0x06);
}
