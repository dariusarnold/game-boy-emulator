#include "bootrom.hpp"
#include "exceptions.hpp"
#include "memorymap.hpp"
#include <fmt/format.h>


BootRom::BootRom(Emulator* emulator, std::array<uint8_t, 256> rom) :
        m_emulator(emulator), m_rom(rom) {}

uint8_t BootRom::read_byte(uint16_t address) const {
    if (!memmap::isIn(address, memmap::BootRom)) {
        throw LogicError(fmt::format("Invalid read from boot rom at {:04X}", address));
    }
    address -= memmap::BootRomBegin;
    return m_rom[address];
}
