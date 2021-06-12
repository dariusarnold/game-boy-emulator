#include "incrementpc.hpp"

void IncrementPC::increment() {
    m_pc.set(m_pc.get() + 1);
}

IncrementPC::IncrementPC(MutableRegister<PC> pc) : m_pc(pc) {}
