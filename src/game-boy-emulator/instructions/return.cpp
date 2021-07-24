#include "return.hpp"

Return::Return(MutableStack s, ProgramCounterJump pc): stack(s), program_counter(pc) {}

unsigned int Return::execute() {
    // PC=(SP), SP=SP+2
    program_counter.set(stack.pop());
    return 16;
}
