#include "mmu.hpp"


uint8_t Mmu::read_memory(u_int16_t address) {
    return ram[address];
}

void Mmu::write_memory(uint16_t address, uint8_t value) {
    ram[address] = value;
}

void Mmu::map_boot_rom(const std::array<uint8_t, constants::BOOT_ROM_SIZE>& boot_rom) {
    std::copy(boot_rom.begin(), boot_rom.end(), ram.begin());
}

uint16_t Mmu::read_memory_word(uint16_t address) {
    auto low_byte = read_memory(address);
    auto high_byte = read_memory(address + 1);
    return (high_byte << constants::BYTE_SIZE) + low_byte;
}
