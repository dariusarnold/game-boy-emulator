#pragma once

#include "facades/register.hpp"
#include "facades/stack.hpp"

/**
 * Push program counter to stack pointer, set program counter to 16 bit location.
 */
class CallIntermediary {
    MutableStack stack;
    MutableRegister<registers::PC> register_pc;
    Memory memory;

public:
    CallIntermediary(Memory mem, MutableStack push_stack, MutableRegister<registers::PC> reg_pc);
    unsigned int execute();
};
