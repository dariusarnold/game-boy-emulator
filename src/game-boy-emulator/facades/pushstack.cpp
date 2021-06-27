#include "pushstack.hpp"
#include "bitmanipulation.hpp"
#include "memory.hpp"
#include "register.hpp"


MutableStack::MutableStack(MutableMemory m, MutableRegister<registers::SP> sp_register): mem(m), sp(sp_register) {}

void MutableStack::push(uint16_t value) {
    sp.set(sp.get() - 1);
    mem.write_byte(sp.get(), bitmanip::get_high_byte(value));
    sp.set(sp.get() - 1);
    mem.write_byte(sp.get(), bitmanip::get_low_byte(value));
}

uint16_t MutableStack::peek() {
    auto low_byte = mem.read_byte(sp.get());
    auto high_byte = mem.read_byte(sp.get() + 1);
    return (high_byte << constants::BYTE_SIZE) + low_byte;
}

uint16_t MutableStack::pop() {
    auto value = peek();
    sp.set(sp.get() + 2);
    return value;
}
