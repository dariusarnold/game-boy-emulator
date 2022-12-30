#pragma once

#include "mbc.hpp"

class Mbc5 : public Mbc {

    // Registers
    // RAM Enable (0x0000-0x1FFF)
    bool m_ram_enable = false;
    // 8 least significant bits of ROM bank number, (0x2000-0x2FFF)
    uint8_t m_rom_bank_number_low = 0;
    // 1 most significant bit of ROM bank number, (0x3000-0x3FFF)
    uint8_t m_rom_bank_number_high = 0;
    // RAM bank number. (0x4000-0x5FFF)
    uint8_t m_ram_bank_number = 0;

    void write_registers(uint16_t address, uint8_t value);
    void write_values(uint16_t address, uint8_t value);

    uint16_t get_rom_bank_number() const;

public:
    using Mbc::Mbc;
    [[nodiscard]] uint8_t read_byte(uint16_t address) const override;
    void write_byte(uint16_t address, uint8_t value) override;
};
