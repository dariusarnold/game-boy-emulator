#include "addressbus.hpp"

#include "bootrom.hpp"
#include "emulator.hpp"
#include "exceptions.hpp"
#include "gpu.hpp"
#include "memorymap.hpp"
#include "mmu.hpp"
#include "ram.hpp"


AddressBus::AddressBus(Emulator* emulator) : m_emulator(emulator) {}

uint8_t AddressBus::read_byte(uint16_t address) const {
    if (m_emulator->is_booting() && memmap::isIn(address, memmap::BootRom)) {
        return m_emulator->get_boot_rom()->read_byte(address);
    } else if (memmap::isIn(address, memmap::InternalRamBank0)
               || memmap::isIn(address, memmap::HighRam)) {
        return m_emulator->get_ram()->read_byte(address);
    } else if (memmap::isIn(address, memmap::BGMapData)
               || memmap::isIn(address, memmap::CharacterRam)) {
        //        return m_emulator->get_gpu()->read_byte(address);
        return 0;
    }
    throw NotImplementedError(fmt::format("Addressing unmapped memory byte at {:04X}", address));
}

uint16_t AddressBus::read_word(uint16_t address) const {
    throw NotImplementedError(fmt::format("Addressing unmapped memory word at {:04X}", address));
}

void AddressBus::write_byte(uint16_t address, uint8_t value) {}

void AddressBus::write_word(uint16_t address, uint16_t value) {}