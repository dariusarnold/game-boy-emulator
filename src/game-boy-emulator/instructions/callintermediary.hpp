#pragma once

#include "facades/pushstack.hpp"
#include "facades/register.hpp"

/**
 * Push program counter to stack pointer, set program counter to 16 bit location.
 */
class CallIntermediary {
    MutableStack stack;
    MutableRegister<PC> register_pc;
    Memory memory;

public:
    CallIntermediary(Memory mem, MutableStack push_stack, MutableRegister<PC> reg_pc);
    int execute();
};
