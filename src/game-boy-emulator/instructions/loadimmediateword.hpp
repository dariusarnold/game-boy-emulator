#pragma once
#include "facades/register.hpp"
#include "facades/memory.hpp"
#include "facades/stack.hpp"


/**
 * Load word immediate data and write it to target_register.
 * @tparam T
 */
template <typename T>
class LoadImmediateWord {
private:
    MutableRegister<T> target_register;
    MutableRegister<registers::PC> register_pc;
    MutableStack stack;
    Memory memory;

public:
    LoadImmediateWord(MutableRegister<T> reg_target, MutableRegister<registers::PC> reg_pc,
                      MutableStack stack, Memory mem) :
            target_register(reg_target), register_pc(reg_pc), stack(stack), memory(mem) {}

    unsigned int execute() {
        auto immediate_data = memory.read_word(register_pc.get());
        register_pc.increment();
        register_pc.increment();
        target_register.set(immediate_data);
        return 12;
    }
};
