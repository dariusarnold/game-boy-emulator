#pragma once

#include <cstdint>
class Emulator;

/**
 * Dispatch memory access to the correct component.
 */
class AddressBus {
    Emulator* m_emulator;
public:
    AddressBus(Emulator* emulator);

    /**
     * Read memory value from address.
     */
    [[nodiscard]] uint8_t read_byte(uint16_t address) const;

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
     * Write 2 byte word to ram at address.
     */
    void write_word(uint16_t address, uint16_t value);
};