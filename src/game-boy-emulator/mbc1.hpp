#pragma once

#include "mbc.hpp"

class Mbc1 : public Mbc {

    // Registers
    // RAM Enable (0x0000-0x1FFF)
    bool m_ram_enable = false;
    // Only 5 bits, (0x2000-0x3FFF)
    uint8_t m_bank1 = 0b00001;
    // Only 2 bits, (0x4000-0x5FFF)
    uint8_t m_bank2 = 0;
    // Only 1 bit. Decides behaviour of RAM bank number. (0x6000-0x7FFF)
    uint8_t m_banking_mode_select = 0;

    void write_registers(uint16_t address, uint8_t value);
    void write_values(uint16_t address, uint8_t value);

public:
    using Mbc::Mbc;
    [[nodiscard]] uint8_t read_byte(uint16_t address) const override;
    void write_byte(uint16_t address, uint8_t value) override;
};
