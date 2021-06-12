#pragma once

#include "constants.h"
#include <array>
#include <cstdint>


class Mmu {
    std::array<uint8_t, constants::RAM_SIZE> ram = {0};

public:
    /**
     * Read memory value from address.
     */
    [[nodiscard]] uint8_t read_memory(u_int16_t address);

    /**
     * Read word from address.
     * Assumes that words are stored least significant byte first
     */
    [[nodiscard]] uint16_t read_memory_word(uint16_t address);

    /**
     * Write value to ram at address.
     */
    void write_memory(uint16_t address, uint8_t value);

    /**
     * Map boot rom at memory address 0x00
     */
    void map_boot_rom(const std::array<uint8_t, constants::BOOT_ROM_SIZE>& boot_rom);
};
