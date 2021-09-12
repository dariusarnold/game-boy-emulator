#include "catch2/catch.hpp"

#include "instructions/callintermediary.hpp"

using namespace registers;

class CallTestFixture {
protected:
    Mmu mmu;
    Memory memory{mmu};
    MutableMemory mutable_memory{mmu};
    uint16_t sp = 0;
    MutableRegister<SP> register_sp{sp};
    MutableStack stack{mutable_memory, register_sp};
    uint16_t pc = 0;
    MutableRegister<PC> register_pc{pc};

    /**
     * Writes target location in place of immediate data after pc.
     * Call this only after setting the program counter
     */
    void set_target_location(uint16_t location) {
        mmu.write_word(pc, location);
    }
};

TEST_CASE_METHOD(CallTestFixture, "Test call instruction setting program counter to location") {
    pc = 0x1234;
    set_target_location(0xABCD);
    CallIntermediary call{memory, stack, register_pc};
    auto cycles = call.execute();
    CHECK(cycles == 24);
    CHECK(register_pc.get() == 0xABCD);
    // stack is set to address of the following instruction, since this instruction has a size of
    // 3 bytes and when entering CallIntermediary the pc is pointing to the first byte of immediate
    // data, we have to check for pc + 2 here
    CHECK(stack.peek() == 0x1234 + 0x02);
    CHECK(mmu.read_word(sp) == 0x1234 + 0x02);
}
