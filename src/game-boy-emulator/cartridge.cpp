#include "cartridge.hpp"

#include <utility>

#include "emulator.hpp"

Cartridge::Cartridge(Emulator* emulator, std::vector<uint8_t> rom) :
        m_rom(std::move(rom)), m_emulator(emulator) {}

uint8_t Cartridge::read_byte(uint16_t address) const {
    return m_rom[address];
}
