#include "memory.hpp"


Memory::Memory(const Mmu& mmu) : m_mmu(mmu) {}

uint8_t Memory::read_byte(uint16_t address) {
    return m_mmu.read_memory(address);
}

uint16_t Memory::read_word(uint16_t address) {
    return m_mmu.read_memory_word(address);
}

MutableMemory::MutableMemory(Mmu& mmu): m_mmu(mmu) {}

uint8_t MutableMemory::read_byte(uint16_t address) {
    return m_mmu.read_memory(address);
}

void MutableMemory::write_byte(uint16_t address, uint8_t value) {
    m_mmu.write_memory(address, value);
}

void MutableMemory::write_word(uint16_t address, uint16_t value) {
    m_mmu.write_memory_word(address, value);
}
