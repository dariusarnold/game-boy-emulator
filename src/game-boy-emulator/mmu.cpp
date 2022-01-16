#include "mmu.hpp"
#include "bitmanipulation.hpp"

#include <iostream>

uint8_t Mmu::read_byte(u_int16_t address) const {
    if (shadow_ram[address] == unitialized) {
        std::cout << "Uninitialized read from " << address << std::endl;
    }
    return ram[address];
}

void Mmu::write_byte(uint16_t address, uint8_t value) {
    shadow_ram[address] = initialized;
    ram[address] = value;
}

void Mmu::map_boot_rom(const std::array<uint8_t, constants::BOOT_ROM_SIZE>& boot_rom) {
    std::copy(boot_rom.begin(), boot_rom.end(), ram.begin());
    std::fill_n(shadow_ram.begin(), boot_rom.size(), initialized);
}

uint16_t Mmu::read_word(uint16_t address) const {
    auto low_byte = read_byte(address);
    auto high_byte = read_byte(address + 1);
    return (high_byte << constants::BYTE_SIZE) + low_byte;
}

void Mmu::write_word(uint16_t address, uint16_t value) {
    write_byte(address, bitmanip::get_low_byte(value));
    write_byte(address + 1, bitmanip::get_high_byte(value));
}
