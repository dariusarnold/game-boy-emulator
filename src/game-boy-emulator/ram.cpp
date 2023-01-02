#include "ram.hpp"
#include "bitmanipulation.hpp"
#include "emulator.hpp"
#include "exceptions.hpp"

#include <fmt/format.h>
#include "spdlog/spdlog.h"

Ram::Ram(Emulator* emulator) : m_emulator(emulator), m_logger(spdlog::get("")) {}

uint8_t Ram::read_byte(uint16_t address) const {
    if (memmap::is_in(address, memmap::InternalRam)) {
        address -= memmap::InternalRamBegin;
        return internalRam[address];
    }
    if (memmap::is_in(address, memmap::HighRam)) {
        address -= memmap::HighRamBegin;
        return highRam[address];
    }
    throw LogicError(fmt::format("Invalid ram read of address {:04X}", address));
}

void Ram::write_byte(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::InternalRam)) {
        address -= memmap::InternalRamBegin;
        internalRam[address] = value;
    } else if (memmap::is_in(address, memmap::HighRam)) {
        address -= memmap::HighRamBegin;
        highRam[address] = value;
    } else {
        throw LogicError(
            fmt::format("Invalid ram write of {:02X} to address {:04X}", value, address));
    }
}
