#pragma once

#include "facades/stack.hpp"
#include "facades/programcounter.hpp"


class Return {
    MutableStack stack;
    ProgramCounterJump program_counter;

public:
    Return(MutableStack s, ProgramCounterJump pc);
    unsigned int execute();
};
