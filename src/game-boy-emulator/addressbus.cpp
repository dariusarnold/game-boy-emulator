#include "addressbus.hpp"

#include "emulator.hpp"
#include "exceptions.hpp"
#include "memorymap.hpp"
#include "mmu.hpp"
#include "bootrom.hpp"
#include "ram.hpp"


AddressBus::AddressBus(Emulator* emulator) : m_emulator(emulator) {}

uint8_t AddressBus::read_byte(uint16_t address) const {
    if (m_emulator->is_booting() && memmap::isIn(address, memmap::BootRom)) {
        return m_emulator->get_boot_rom()->read_byte(address);
    } else if (memmap::isIn(address, memmap::InternalRamBank0)
               || memmap::isIn(address, memmap::HighRam)) {
        return m_emulator->get_ram()->read_byte(address);
    }
    throw NotImplementedError(fmt::format("Addressing bus byte at {:04X}", address));
}

uint16_t AddressBus::read_word(uint16_t address) const {
    throw NotImplementedError(fmt::format("Addressing bus wird at {:04X}", address));
}

void AddressBus::write_byte(uint16_t address, uint8_t value) {
}

void AddressBus::write_word(uint16_t address, uint16_t value) {
}
