#include "cartridge.hpp"

#include "emulator.hpp"

Cartridge::Cartridge(Emulator* emulator, const std::vector<uint8_t>& rom) :
        m_emulator(emulator), m_rom(rom) {}

uint8_t Cartridge::read_byte(uint16_t address) const {
    return m_rom[address];
}
