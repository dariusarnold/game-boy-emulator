#pragma once

#include "mbc.hpp"

class Mbc1 : public Mbc {

    // Registers
    // RAM Enable (0x0000-0x1FFF)
    uint8_t m_ramg = 0;
    // Only 5 bits, (0x2000-0x3FFF)
    uint8_t m_bank1 = 0b00001;
    // Only 2 bits, (0x4000-0x5FFF)
    uint8_t m_bank2 = 0;
    // Only 1 bit. Decides behaviour of RAM bank number. (0x6000-0x7FFF)
    uint8_t m_banking_mode_select = 0;

    // Number of bits required for an address into ROM/RAM. If ROM/RAM is less than the maximum
    // size, higher bits will be ignored when addressing into them.
    int m_required_rom_bits = 0;
    int m_required_ram_bits = 0;

    void write_registers(uint16_t address, uint8_t value);
    void write_values(uint16_t address, uint8_t value);

    uint32_t get_address_in_rom(uint16_t address, uint32_t bank_number) const;
    uint32_t get_address_in_ram(uint16_t address) const;

public:
    Mbc1(std::vector<uint8_t> rom, std::span<uint8_t> ram);
    [[nodiscard]] uint8_t read_byte(uint16_t address) const override;
    void write_byte(uint16_t address, uint8_t value) override;
};
