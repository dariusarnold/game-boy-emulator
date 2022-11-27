#include "addressbus.hpp"

#include "bootrom.hpp"
#include "cartridge.hpp"
#include "emulator.hpp"
#include "exceptions.hpp"
#include "gpu.hpp"
#include "memorymap.hpp"
#include "ram.hpp"

#include "fmt/format.h"


AddressBus::AddressBus(Emulator* emulator) : m_emulator(emulator) {}

uint8_t static io_registerFF42 = 0x64;

uint8_t AddressBus::read_byte(uint16_t address) const {
    if (m_emulator->is_booting() && memmap::is_in(address, memmap::BootRom)) {
        return m_emulator->get_boot_rom()->read_byte(address);
    } else if (memmap::is_in(address, memmap::CartridgeRom)) {
        return m_emulator->get_cartridge()->read_byte(address);
    } else if (memmap::is_in(address, memmap::InternalRamBank0)
               || memmap::is_in(address, memmap::HighRam)) {
        return m_emulator->get_ram()->read_byte(address);
    } else if (memmap::is_in(address, memmap::VRam)) {
        return m_emulator->get_gpu()->read_byte(address);
    } else if (memmap::is_in(address, memmap::IORegisters)) {
        // TODO Those special cases are required for booting correctly
        if (address == 0xFF44) {
            return 0x90;
        } else if (address == 0xFF42) {
            return io_registerFF42;
        }
        fmt::print("IGNORED: read from IO register {:04X}\n", address);
        return 0xFF;
    }
    throw NotImplementedError(fmt::format("Addressing unmapped memory byte at {:04X}", address));
}

void AddressBus::write_byte(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::InternalRamBank0) || memmap::is_in(address, memmap::HighRam)) {
        m_emulator->get_ram()->write_byte(address, value);
    } else if (memmap::is_in(address, memmap::VRam)) {
        m_emulator->get_gpu()->write_byte(address, value);
    } else if(memmap::is_in(address, memmap::IORegisters)) {
        if (m_emulator->is_booting()) {
            if (address == 0xFF50) {
                m_emulator->signal_boot_ended();
            }
        }
        if (address == 0xFF42) {
            io_registerFF42 = value;
        }
        fmt::print("IGNORED: write to IO registers {:04X}\n", address);
    } else {
        throw NotImplementedError(fmt::format("Writing unmapped memory byte at {:04X}", address));
    }
}
