#include "incrementpc.hpp"
#include "register.hpp"

void IncrementPC::increment() {
    m_pc.set(m_pc.get() + 1);
}

using namespace registers;

IncrementPC::IncrementPC(MutableRegister<PC> pc) : m_pc(pc) {}
