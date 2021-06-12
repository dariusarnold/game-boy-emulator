#pragma once

#include "register.hpp"

/**
 * Facade for incrementing program counter.
 */
class IncrementPC {
    MutableRegister<PC> m_pc;

public:
    explicit IncrementPC(MutableRegister<PC> pc);
    void increment();
};