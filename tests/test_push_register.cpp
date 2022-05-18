#include "catch2/catch.hpp"
#include "instructions/pushregister.hpp"

using namespace registers;

class PushRegisterFixture {
protected:
    uint16_t bc = 0x1234;
    Register<BC> register_bc{bc};
    // Stack grows down, so we have to use a value != 0 here to avoid underflow
    uint16_t sp = 0x08;
    Register<SP> register_sp{sp};
    Mmu mmu;
    MutableMemory memory{mmu};
    MutableStack stack{memory, register_sp};
};


TEST_CASE_METHOD(PushRegisterFixture, "Pushing register to stack") {
    PushRegisterOnStack<BC> push_register_on_stack{register_bc, stack};
    auto cycles = push_register_on_stack.execute();
    CHECK(cycles == 16);
    CHECK(stack.peek() == bc);
    CHECK(sp == 0x06);
}
