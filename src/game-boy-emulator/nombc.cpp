#include <cassert>
#include "nombc.hpp"
#include "memorymap.hpp"
#include "exceptions.hpp"
#include "spdlog/logger.h"

uint8_t NoMbc::read_byte(uint16_t address) const {
    if (memmap::is_in(address, memmap::CartridgeRom)) {
        return get_rom()[address];
    }
    if (get_ram().empty()) {
        get_logger()->error("Read from nonexisting RAM");
        return 0xFF;
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
        get_logger()->error("Write to ROM without MBC");
    } else if (memmap::is_in(address, memmap::CartridgeRam)) {
        if (get_ram().empty()) {
            return;
        }
        const uint16_t relative_address = address - memmap::CartridgeRamBegin;
        assert(relative_address < get_ram().size() && "No MBC write out of bounds");
        get_ram()[relative_address] = value;
    }
}
