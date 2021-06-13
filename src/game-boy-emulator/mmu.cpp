#include "mmu.hpp"
#include "bitmanipulation.hpp"


uint8_t Mmu::read_memory(u_int16_t address) const {
    return ram[address];
}

void Mmu::write_memory(uint16_t address, uint8_t value) {
    ram[address] = value;
}

void Mmu::map_boot_rom(const std::array<uint8_t, constants::BOOT_ROM_SIZE>& boot_rom) {
    std::copy(boot_rom.begin(), boot_rom.end(), ram.begin());
}

uint16_t Mmu::read_memory_word(uint16_t address) const {
    auto low_byte = read_memory(address);
    auto high_byte = read_memory(address + 1);
    return (high_byte << constants::BYTE_SIZE) + low_byte;
}

void Mmu::write_memory_word(uint16_t address, uint16_t value) {
    write_memory(address, bitmanip::get_low_byte(value));
    write_memory(address + 1, bitmanip::get_high_byte(value));
}
