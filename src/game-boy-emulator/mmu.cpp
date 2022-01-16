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
    // The logo should be stored on the cartridge, but for now just use this since it is required
    // for booting.
    const std::array<uint8_t, 0x30> logo{0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73,
                                         0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F,
                                         0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD,
                                         0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
                                         0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E};
    uint16_t starting_address{0x104};
    std::for_each(logo.begin(), logo.end(), [&, address = starting_address](uint8_t value) mutable {
        write_byte(address, value);
        ++address;
    });
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
