#pragma once

#pragma once

#include "constants.h"
#include <array>
#include <cstdint>


class IMmu {
public:
    /**
     * Read memory value from address.
     */
    [[nodiscard]] virtual uint8_t read_byte(u_int16_t address) const = 0;

    /**
     * Read word from address.
     * Assumes that words are stored least significant byte first
     */
    [[nodiscard]] virtual uint16_t read_word(uint16_t address) const = 0;

    /**
     * Write value to ram at address.
     */
    virtual void write_byte(uint16_t address, uint8_t value) = 0;

    /**
     * Write 2 byte wird to ram at address.
     */
    virtual void write_word(uint16_t address, uint16_t value) = 0;
};
