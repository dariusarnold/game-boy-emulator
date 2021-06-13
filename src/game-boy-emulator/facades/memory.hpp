#pragma once

#include "mmu.hpp"

/**
 * Facade for reading memory
 */
class Memory {
    const Mmu& m_mmu;

public:
    explicit Memory(const Mmu& mmu);

    uint8_t read_byte(uint16_t address);

    uint16_t read_word(uint16_t address);
};

class MutableMemory {
    Mmu& m_mmu;

public:
    explicit MutableMemory(Mmu& mmu);

    uint8_t read_byte(uint16_t address);

    void write_byte(uint16_t address, uint8_t value);

    void write_word(uint16_t address, uint16_t value);
};