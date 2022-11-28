#include "bootrom.hpp"
#include "exceptions.hpp"
#include "memorymap.hpp"
#include "constants.h"
#include <fmt/format.h>


BootRom::BootRom(Emulator* emulator, std::array<uint8_t, constants::BOOT_ROM_SIZE> rom) :
        m_emulator(emulator), m_rom(rom) {}

uint8_t BootRom::read_byte(uint16_t address) const {
    if (!memmap::is_in(address, memmap::BootRom)) {
        throw LogicError(fmt::format("Invalid read from boot rom at {:04X}", address));
    }
    address -= memmap::BootRomBegin;
    return m_rom[address];
}
