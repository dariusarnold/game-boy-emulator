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

uint16_t Ram::read_word(uint16_t address) const {
    auto low_byte = read_byte(address);
    auto high_byte = read_byte(address + 1);
    return bitmanip::word_from_bytes(high_byte, low_byte);
}

void Ram::write_byte(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::InternalRam)) {
        if (address == 0xDD01) {
            m_logger->error("Write {:02X} to DD01", value);
        }
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
void Ram::write_word(uint16_t address, uint16_t value) {
    write_byte(address, bitmanip::get_low_byte(value));
    write_byte(address + 1, bitmanip::get_high_byte(value));
}
