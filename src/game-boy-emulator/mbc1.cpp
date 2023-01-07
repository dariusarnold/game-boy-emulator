#include "mbc1.hpp"

#include "memorymap.hpp"
#include "bitmanipulation.hpp"
#include "exceptions.hpp"
#include "fmt/format.h"
#include "spdlog/spdlog.h"
#include <cassert>


namespace {
constexpr uint8_t RAM_ENABLE_VALUE = 0xA;
} // namespace

uint32_t Mbc1::get_address_in_rom(uint16_t address, uint32_t bank_number) const {
    auto rom_file_address = (bank_number << 14) | (address & 0b11111111111111);
    // For small ROM cartridges bits of the address which are not within the actual ROMs size are
    // ignored.
    rom_file_address = bitmanip::mask(rom_file_address, m_required_rom_bits);
    return rom_file_address;
}

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
        if (m_ramg != RAM_ENABLE_VALUE) {
            get_logger()->error("Read from disabled cartridge RAM in MBC1");
            return 0xFF;
        }
        uint32_t address_in_ram = get_address_in_ram(address);
        assert(address_in_ram < get_ram().size() && "Read to cartridge RAM bank out of bounds");
        return get_ram()[address_in_ram];
    }
    throw LogicError(fmt::format("Cartridge trying to read from {:04X}", address));
}

void Mbc1::write_byte(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::CartridgeRam)) {
        write_values(address, value);
    } else {
        write_registers(address, value);
    }
}

void Mbc1::write_registers(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::RamEnable)) {
        m_ramg = bitmanip::get_low_nibble(value);
    } else if (memmap::is_in(address, memmap::RomBankNumber)) {
        // BANK1, select rom bank number for CartridgeRomBankSwitchable
        value = bitmanip::mask(value, 5);
        if (value == 0) {
            // Rom bank X0 can't be selected. Selecting it will select rom bank X1
            value += 1;
        }
        m_bank1 = value;
    } else if (memmap::is_in(address, memmap::RamBankNumber)) {
        m_bank2 = value & 0b11;
    } else if (memmap::is_in(address, memmap::BankingModeSelect)) {
        m_banking_mode_select = value & 1;
    }
    get_logger()->debug("Cartridge registers: RAMG {:02X}, BANK1 {:05B}, BANK2 {:02b}, MODE {:1B}",
                        m_ramg, m_bank1, m_bank2, m_banking_mode_select);
}

uint32_t Mbc1::get_address_in_ram(uint16_t address) const {
    if (m_banking_mode_select == 0) {
        // In simple banking mode only bank 0 can be accessed
        unsigned address_in_ram = address - memmap::CartridgeRamBegin;
        address_in_ram = bitmanip::mask(address_in_ram, m_required_ram_bits);
        return address_in_ram;
    }
    unsigned address_in_ram = address + static_cast<unsigned>(m_bank2 * memmap::CartridgeRamSize)
                              - static_cast<unsigned>(memmap::CartridgeRamBegin);
    address_in_ram = bitmanip::mask(address_in_ram, m_required_ram_bits);
    return address_in_ram;
}

void Mbc1::write_values(uint16_t address, uint8_t value) {
    // Actual RAM writes
    if (m_ramg != RAM_ENABLE_VALUE) {
        get_logger()->warn("Write to {:04X} with disabled ram was ignored", address);
        return;
    }
    auto address_in_ram = get_address_in_ram(address);
    assert(address_in_ram < get_ram().size() && "Write to cartridge RAM bank out of bounds");
    get_ram()[address_in_ram] = value;
}

Mbc1::Mbc1(std::vector<uint8_t> rom, std::span<uint8_t> ram) :
        Mbc(std::move(rom), ram),
        m_required_rom_bits(std::log2(get_rom_info().size_bytes)),
        m_required_ram_bits(std::log2(get_ram_info().size_bytes)) {}
