#include "addressbus.hpp"

#include "emulator.hpp"
#include "mmu.hpp"

AddressBus::AddressBus(Emulator* emulator): m_emulator(emulator){}


uint8_t AddressBus::read_byte(uint16_t address) const {
    return m_emulator->get_mmu()->read_byte(address);
}

uint16_t AddressBus::read_word(uint16_t address) const {
    return m_emulator->get_mmu()->read_word(address);
}

void AddressBus::write_byte(uint16_t address, uint8_t value) {
    m_emulator->get_mmu()->write_byte(address, value);
}

void AddressBus::write_word(uint16_t address, uint16_t value) {
    m_emulator->get_mmu()->write_word(address, value);
}
