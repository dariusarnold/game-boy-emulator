#pragma once

#include "register.hpp"

/**
 * Facade for incrementing program counter.
 */
class IncrementPC {
    MutableRegister<registers::PC> m_pc;

public:
    explicit IncrementPC(MutableRegister<registers::PC> pc);
    void increment();
};