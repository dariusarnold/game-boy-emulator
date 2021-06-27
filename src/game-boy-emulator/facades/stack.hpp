#pragma once

#include "facades/memory.hpp"
#include "facades/register.hpp"
#include "register.hpp"
#include <cstdint>


/**
 * Facade for stack access
 */
class MutableStack {
    MutableMemory mem;
    MutableRegister<registers::SP> sp;

public:
    MutableStack(MutableMemory m, MutableRegister<registers::SP> sp_register);
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
