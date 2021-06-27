#pragma once

#include "facades/programcounter.hpp"
#include "facades/register.hpp"
#include "facades/stack.hpp"

/**
 * Push value of word register on stack
 * @tparam T Which combined register (BC, DE, HL, AF) to use
 */
template <typename T>
class PushRegisterOnStack {
    static_assert(registers::is_word_register_v<T>);
    Register<T> reg;
    MutableStack stack;
    ProgramCounterIncDec increment_pc;

public:
    PushRegisterOnStack(Register<T> r, MutableStack s, ProgramCounterIncDec ipc) :
            reg(r), stack(s), increment_pc(ipc) {}
    unsigned int execute() {
        stack.push(reg.get());
        increment_pc.increment();
        return 16;
    }
};
