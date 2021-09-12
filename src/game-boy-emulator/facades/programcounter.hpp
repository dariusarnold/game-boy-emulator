#pragma once

#include "register.hpp"

/**
 * Facade for incrementing/decrementing program counter.
 */
class ProgramCounterIncDec {
protected:
    MutableRegister<registers::PC> m_pc;

public:
    explicit ProgramCounterIncDec(MutableRegister<registers::PC> pc);
    void increment();
    void decrement();
};

/**
 * Facade for setting program counter to new value.
 */
class ProgramCounterJump: public ProgramCounterIncDec {
public:
    using ProgramCounterIncDec::ProgramCounterIncDec;
    void set(uint16_t value);
};