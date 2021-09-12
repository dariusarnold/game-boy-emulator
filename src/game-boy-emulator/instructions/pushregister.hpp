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

public:
    PushRegisterOnStack(Register<T> r, MutableStack s) :
            reg(r), stack(s) {}

    unsigned int execute() {
        stack.push(reg.get());
        return 16;
    }
};
