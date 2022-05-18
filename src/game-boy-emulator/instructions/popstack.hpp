#pragma once

#include "facades/programcounter.hpp"
#include "facades/register.hpp"
#include "facades/stack.hpp"


template <typename T>
class PopStack {
    Register<T> target_register;
    MutableStack stack;

public:
    PopStack(Register<T> target_register_, MutableStack stack_): target_register(target_register_), stack(stack_) {}

    unsigned int execute() {
        auto value = stack.pop();
        target_register.set(value);
        return 12;
    }
};
