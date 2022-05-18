#include "instructions/return.hpp"
#include <catch2/catch.hpp>

class ReturnTestFixture {
protected:
    Mmu mmu;
    MutableMemory mem{mmu};
    uint16_t sp = 0;
    Register<registers::SP> register_sp{sp};
    MutableStack stack{mem, register_sp};
    uint16_t pc = 0;
    Register<registers::PC> register_pc{pc};
    ProgramCounterJump program_counter{register_pc};
};

TEST_CASE_METHOD(ReturnTestFixture, "Test return instruction") {
    // Setup: Push two addresses on stack. The first one (0xDEAD) will be the one at the top
    // of the stack after returning and the second one should be the new value of PC.
    sp = 0x1234;
    stack.push(0xDEAD);
    stack.push(0xBEEF);
    Return r{stack, program_counter};
    auto cycles = r.execute();
    CHECK(cycles == 16);
    // Check if program counter points to new value, which was on top of the stack
    CHECK(pc == 0xBEEF);
    // Check if stack was popped
    CHECK(stack.peek() == 0xDEAD);
}