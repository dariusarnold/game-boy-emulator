#pragma once

#include "facades/register.hpp"
#include "facades/stack.hpp"

/**
 * Push program counter to stack pointer, set program counter to 16 bit location.
 */
class CallImmediate {
    MutableStack stack;
    Register<registers::PC> register_pc;
    Memory memory;

public:
    CallImmediate(Memory mem, MutableStack push_stack, Register<registers::PC> reg_pc);
    unsigned int execute();
};
