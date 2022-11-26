#include "ram.hpp"
#include "bitmanipulation.hpp"
#include "emulator.hpp"
#include "exceptions.hpp"

Ram::Ram(Emulator* emulator) : m_emulator(emulator) {}

uint8_t Ram::read_byte(uint16_t address) const {
    if (memmap::isIn(address, memmap::InternalRamBank0)) {
        address -= memmap::InternalRamBank0Begin;
        return internalRam[address];
    } else if (memmap::isIn(address, memmap::HighRam)) {
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
    if (memmap::isIn(address, memmap::InternalRamBank0)) {
        address -= memmap::InternalRamBank0Begin;
        internalRam[address] = value;
    } else if (memmap::isIn(address, memmap::HighRam)) {
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
