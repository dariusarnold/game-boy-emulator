#pragma once

#pragma once

#include "constants.h"
#include <array>
#include <cstdint>


class IMemoryAccess {
public:
    /**
     * Read memory value from address.
     */
    [[nodiscard]] virtual uint8_t read_byte(uint16_t address) const = 0;

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
     * Write 2 byte word to ram at address.
     */
    virtual void write_word(uint16_t address, uint16_t value) = 0;
};

class IMemoryRange: public IMemoryAccess{
public:
    [[nodiscard]] virtual uint16_t get_begin_address() const = 0;
    [[nodiscard]] virtual uint16_t get_end_address() const = 0;
    [[nodiscard]] virtual std::pair<uint16_t, uint16_t> get_mapped_range() const {
        return {get_begin_address(), get_end_address()};
    };
    [[nodiscard]] virtual bool is_address_in_mapped_range(uint16_t address) const {
        return address <= get_end_address() && address >= get_begin_address();
    };
    virtual ~IMemoryRange() = default;
};