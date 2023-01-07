#include "mbc1.hpp"

#include "memorymap.hpp"
#include "bitmanipulation.hpp"
#include "exceptions.hpp"
#include "fmt/format.h"
#include "spdlog/spdlog.h"
#include <cassert>


namespace {
bool decide_ram_enable(uint8_t value) {
    // Any value with 0xA as lower nibble enables the ram, every other value disables it.
    return bitmanip::get_low_nibble(value) == 0xA;
}

uint32_t get_address_in_rom(uint16_t address, uint32_t bank_number) {
    return (bank_number << 14) | (address & 0b11111111111111);
}
} // namespace

uint8_t Mbc1::read_byte(uint16_t address) const {
    if (memmap::is_in(address, memmap::CartridgeRomFixedBank)) {
        uint32_t bank_number = 0;
        if (m_banking_mode_select == 1) {
            bank_number = static_cast<uint32_t>(m_bank2 << 5);
        }
        uint32_t address_in_rom = get_address_in_rom(address, bank_number);
        assert(address_in_rom < get_rom().size() && "Read ROM fixed bank out of bounds");
        return get_rom()[address_in_rom];
    }
    if (memmap::is_in(address, memmap::CartridgeRomBankSwitchable)) {
        auto bank_number = static_cast<uint32_t>((m_bank2 << 5) | m_bank1);
        uint32_t address_in_rom = get_address_in_rom(address, bank_number);
        assert(address_in_rom < get_rom().size() && "Read ROM switchable bank out of bounds");
        return get_rom()[address_in_rom];
    }
    if (memmap::is_in(address, memmap::CartridgeRam)) {
        if (!m_ram_enable) {
            get_logger()->error("Read from disabled cartridge RAM in MBC1");
            return 0xFF;
        }
        uint32_t bank_number = 0;
        if (m_banking_mode_select == 1) {
            bank_number = static_cast<uint32_t>(m_bank2 << 13);
        }
        uint32_t address_in_ram = bank_number << 13 | (address & 0b1111111111111);
        assert(address_in_ram < get_ram().size() && "Read to cartridge RAM bank out of bounds");
        return get_ram()[address_in_ram];
    }
    throw LogicError(fmt::format("Cartridge trying to read from {:04X}", address));
}

void Mbc1::write_byte(uint16_t address, uint8_t value) {
    write_registers(address, value);
    write_values(address, value);
}

void Mbc1::write_registers(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::RamEnable)) {
        m_ram_enable = decide_ram_enable(value);
    } else if (memmap::is_in(address, memmap::RomBankNumber)) {
        // BANK1, select rom bank number for CartridgeRomBankSwitchable
        if ((value & 0b11111) == 0) {
            // Rom bank X0 can't be selected. Selecting it will select rom bank X1
            value += 1;
        }
        m_bank1 = (value & 0b11111);
    } else if (memmap::is_in(address, memmap::RamBankNumber)) {
        m_bank2 = value & 0b11;
    } else if (memmap::is_in(address, memmap::BankingModeSelect)) {
        m_banking_mode_select = value & 1;
    }
    get_logger()->debug("Cartridge registers: RAMG {}, BANK1 {:05B}, BANK2 {:02b}, MODE {:1B}",
                        m_ram_enable, m_bank1, m_bank2, m_banking_mode_select);
}

void Mbc1::write_values(uint16_t address, uint8_t value) {
    // Actual RAM writes
    if (memmap::is_in(address, memmap::CartridgeRam)) {
        if (!m_ram_enable) {
            get_logger()->warn("Write to {:04X} with disabled ram");
            return;
        }
        if (m_banking_mode_select == 0) {
            // In simple banking mode only bank 0 can be accessed
            address -= memmap::CartridgeRamBegin;
            assert(address < get_ram().size() && "Write to cartridge RAM bank out of bounds");
            get_ram()[address] = value;
        } else {
            address -= memmap::CartridgeRamBegin + m_bank2 * memmap::CartridgeRamSize;
            assert(address < get_ram().size() && "Write to cartridge RAM bank out of bounds");
            get_ram()[address] = value;
        }
    }
}
