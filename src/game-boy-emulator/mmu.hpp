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
    [[nodiscard]] uint8_t read_byte(u_int16_t address) const;

    /**
     * Read word from address.
     * Assumes that words are stored least significant byte first
     */
    [[nodiscard]] uint16_t read_word(uint16_t address) const;

    /**
     * Write value to ram at address.
     */
    void write_byte(uint16_t address, uint8_t value);

    /**
     * Write 2 byte wird to ram at address.
     */
    void write_word(uint16_t address, uint16_t value);

    /**
     * Map boot rom at memory address 0x00
     */
    void map_boot_rom(const std::array<uint8_t, constants::BOOT_ROM_SIZE>& boot_rom);
};
