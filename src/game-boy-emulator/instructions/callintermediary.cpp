#include "callintermediary.hpp"

CallIntermediary::CallIntermediary(Memory mem, MutableStack push_stack, MutableRegister<registers::PC> reg_pc) :
        stack(push_stack), register_pc(reg_pc), memory(mem) {}

unsigned int CallIntermediary::execute() {
    // read 2 byte immediate data
    register_pc.increment();
    auto location = memory.read_word(register_pc.get());
    // Push pc of the following instruction onto stack
    register_pc.increment();
    register_pc.increment();
    stack.push(register_pc.get());
    // set pc to new location
    register_pc.set(location);
    return 24;
}
