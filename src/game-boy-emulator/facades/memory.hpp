#pragma once

#include "mmu.hpp"

/**
 * Facade for reading memory
 */
class Memory {
    const IMemoryAccess& m_mmu;

public:
    explicit Memory(const IMemoryAccess& mmu);

    uint8_t read_byte(uint16_t address);

    uint16_t read_word(uint16_t address);
};

class MutableMemory {
    IMemoryAccess& m_mmu;

public:
    explicit MutableMemory(IMemoryAccess& mmu);

    uint8_t read_byte(uint16_t address);

    void write_byte(uint16_t address, uint8_t value);

    void write_word(uint16_t address, uint16_t value);
};