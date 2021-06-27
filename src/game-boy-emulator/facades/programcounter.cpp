#include "programcounter.hpp"
#include "register.hpp"

void ProgramCounterIncDec::increment() {
    m_pc.set(m_pc.get() + 1);
}

ProgramCounterIncDec::ProgramCounterIncDec(MutableRegister<registers::PC> pc) : m_pc(pc) {}

void ProgramCounterIncDec::decrement() {
    m_pc.set(m_pc.get() - 1);
}

void ProgramCounterJump::set(uint16_t value) {
    m_pc.set(value);
}
