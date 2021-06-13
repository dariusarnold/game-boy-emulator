#pragma once

#include "facades/register.hpp"
#include "facades/memory.hpp"
#include <cstdint>


/**
 * Facade for stack access
 */
class MutableStack {
    MutableMemory mem;
    MutableRegister<SP> sp;

public:
    MutableStack(MutableMemory m, MutableRegister<SP> sp_register);
    /**
     * Push word onto stack.
     */
    void push(uint16_t value);

    /**
     * Return word from stack without popping it.
     */
    uint16_t peek();

    /**
     * Remove word from top of stack and return it.
     */
    uint16_t pop();
};
