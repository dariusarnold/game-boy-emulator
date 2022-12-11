#include "ppu_registers.hpp"
#include "bitmanipulation.hpp"

uint8_t PpuRegisters::get_register_value(PpuRegisters::Register r) {
    return get(r);
}

void PpuRegisters::set_register_value(PpuRegisters::Register r, uint8_t value) {
    get(r) = value;
}

uint8_t PpuRegisters::get_register_value(uint16_t address) {
    return get_register_value(static_cast<Register>(address));
}

void PpuRegisters::set_register_value(uint16_t address, uint8_t value) {
    set_register_value(static_cast<Register>(address), value);
}

PpuRegisters::PpuRegisters(bool fix_ly) {
    if (fix_ly) {
        set_register_value(Register::LyRegister, 0x90);
    }
}

uint8_t& PpuRegisters::get(PpuRegisters::Register r) {
    return m_registers[static_cast<uint16_t>(r) - static_cast<uint16_t>(Register::LcdcRegister)];
}

const uint8_t& PpuRegisters::get(PpuRegisters::Register r) const {
    return m_registers[static_cast<uint16_t>(r) - static_cast<uint16_t>(Register::LcdcRegister)];
}

