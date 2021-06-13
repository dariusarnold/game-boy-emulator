#pragma once

#include "facades/register.hpp"
#include "facades/pushstack.hpp"
#include "facades/incrementpc.hpp"

/**
 * Push value of word register on stack
 * @tparam T Which combined register (BC, DE, HL, AF) to use
 */
template <typename T>
class PushRegisterOnStack {
    static_assert(is_word_register_v<T>);
    Register<T> reg;
    MutableStack stack;
    IncrementPC increment_pc;

public:
    PushRegisterOnStack(Register<T> r, MutableStack s, IncrementPC ipc) :
            reg(r), stack(s), increment_pc(ipc) {}
    int execute() {
        stack.push(reg.get());
        increment_pc.increment();
        return 16;
    }
};
