#include <cassert>
#include "nombc.hpp"
#include "memorymap.hpp"
#include "exceptions.hpp"

uint8_t NoMbc::read_byte(uint16_t address) const {
    if (memmap::is_in(address, memmap::CartridgeRom)) {
        return get_rom()[address];
    }
    if (memmap::is_in(address, memmap::CartridgeRam)) {
        const uint16_t relative_address = address - memmap::CartridgeRamBegin;
        assert(relative_address < get_ram().size() && "No MBC read out of bounds");
        return get_ram()[relative_address];
    }
    throw LogicError("NoMBC read from");
}

void NoMbc::write_byte(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::CartridgeRom)) {
        get_rom()[address] = value;
    } else if (memmap::is_in(address, memmap::CartridgeRam)) {
        const uint16_t relative_address = address - memmap::CartridgeRamBegin;
        assert(relative_address < get_ram().size() && "No MBC write out of bounds");
        get_ram()[address - memmap::CartridgeRamBegin] = value;
    }
}
