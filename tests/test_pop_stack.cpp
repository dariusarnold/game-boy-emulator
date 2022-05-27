#include "catch2/catch.hpp"
#include "instructions/popstack.hpp"

using namespace registers;

class PopStackFixture {
protected:
    uint16_t bc = 0x1234;
    Register<BC> register_bc{bc};
    // Stack grows down, so we have to use a value != 0 here to avoid underflow
    uint16_t sp = 0xC100;
    Register<SP> register_sp{sp};
    Mmu mmu;
    MutableMemory memory{mmu};
    MutableStack stack{memory, register_sp};
};


TEST_CASE_METHOD(PopStackFixture, "Popping 2 bytes from stack into BC register") {
    uint16_t val = 0xABCD;
    // This decrements sp by two
    stack.push(val);
    PopStack<BC> pop_stack{register_bc, stack};
    auto cycles = pop_stack.execute();
    CHECK(cycles == 12);
    CHECK(bc == val);
    CHECK(sp == 0xC100);
}

