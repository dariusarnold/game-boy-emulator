#include "cartridge.hpp"

#include "emulator.hpp"
#include "memorymap.hpp"
#include "exceptions.hpp"
#include "bitmanipulation.hpp"

#include "fmt/format.h"
#include "magic_enum.hpp"

#include <utility>
#include <spdlog/spdlog.h>

const int CARTRIDGE_TYPE_OFFSET = 0x147;
const int ROM_SIZE = 0x148;
const int RAM_SIZE = 0x149;

Cartridge::Cartridge(Emulator* emulator, std::vector<uint8_t> rom) :
        m_rom(std::move(rom)), m_emulator(emulator), m_logger(spdlog::get("")) {
    if (m_rom.size() < memmap::CartridgeHeaderEnd) {
        throw LogicError(
            fmt::format("ROM only {} bytes, does not contain cartridge header", m_rom.size()));
    }
    // Initialize after size check to avoid potential out-of-bounds access.
    m_cartridge_type = get_cartridge_type();
    switch (m_cartridge_type) {
    case CartridgeType::ROM_ONLY:
    case CartridgeType::MBC1:
    case CartridgeType::MBC1_RAM:
        break;
    default:
        throw NotImplementedError(fmt::format("Cartridge type {} not implemented",
                                              magic_enum::enum_name(m_cartridge_type)));
    }
    m_rom_size_info = get_rom_size();
    m_ram_size_info = get_ram_size();
}

namespace {
bool decide_ram_enable(uint8_t value) {
    // Any value with 0xA as lower nibble enables the ram, every other value disables it.
    return bitmanip::get_low_nibble(value) == 0xA;
}

uint32_t get_address_in_rom(uint16_t address, uint32_t bank_number) {
    return (bank_number << 14) | (address & 0b11111111111111);
}
} // namespace

uint8_t Cartridge::read_byte(uint16_t address) const {
    if (memmap::is_in(address, memmap::CartridgeRomFixedBank)) {
        uint32_t bank_number = 0;
        if (m_banking_mode_select == 1) {
            bank_number = static_cast<uint32_t>(m_bank2 << 5);
        }
        uint32_t address_in_rom = get_address_in_rom(address, bank_number);
        assert(address_in_rom < m_rom.size() && "Read ROM fixed bank out of bounds");
        return m_rom[address_in_rom];
    }
    if (memmap::is_in(address, memmap::CartridgeRomBankSwitchable)) {
        uint32_t bank_number = static_cast<uint32_t>((m_bank2 << 5) | m_bank1);
        uint32_t address_in_rom = get_address_in_rom(address, bank_number);
        assert(address_in_rom < m_rom.size() && "Read ROM switchable bank out of bounds");
        return m_rom[address_in_rom];
    }
    if (memmap::is_in(address, memmap::CartridgeRam)) {
        uint32_t bank_number = 0;
        if (m_banking_mode_select == 1) {
            bank_number = static_cast<uint32_t>(m_bank2 << 13);
        }
        uint32_t address_in_rom = bank_number << 13 | (address & 0b1111111111111);
        assert(address_in_rom < m_rom.size() && "Read to cartridge RAM bank out of bounds");
        return m_rom[address_in_rom];
    }
    throw LogicError(fmt::format("Cartridge trying to read from {:04X}", address));
}

void Cartridge::write_byte(uint16_t address, uint8_t value) {
    write_registers(address, value);
    write_values(address, value);
};


Cartridge::CartridgeType Cartridge::get_cartridge_type() const {
    auto val = m_rom[CARTRIDGE_TYPE_OFFSET];
    if (magic_enum::enum_contains<CartridgeType>(val)) {
        return CartridgeType{val};
    }
    throw LogicError(fmt::format("Invalid value {:02X} for cartridge type", val));
}

Cartridge::RomInfo Cartridge::get_rom_size() const {
    auto val = m_rom[ROM_SIZE];
    if (val > 0x8) {
        throw LogicError("Invalid value for ROM size");
    }
    return {(32 * 1024) * (1 << val), 1 << val};
}

Cartridge::RamInfo Cartridge::get_ram_size() const {
    auto val = m_rom[RAM_SIZE];
    switch (val) {
    case 0:
        return {0, 0};
    case 2:
        return {8 * 104, 1};
    case 3:
        return {32 * 1024, 4};
    case 4:
        return {128 * 1024, 16};
    case 5:
        return {64 * 1024, 8};
    default:
        throw LogicError("Invalid value for RAM size");
    }
}

void Cartridge::write_registers(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::RamEnable)) {
        m_ram_enable = decide_ram_enable(value);
    } else if (memmap::is_in(address, memmap::RomBankNumber)) {
        // BANK1
        if ((value & 0b11111) == 0) {
            value += 1;
        }
        m_bank1 = (value & 0b11111);
    } else if (memmap::is_in(address, memmap::RamBankNumber)) {
        m_bank2 = value & 0b11;
    } else if (memmap::is_in(address, memmap::BankingModeSelect)) {
        m_banking_mode_select = value & 1;
    }
    spdlog::debug("Cartridge registers: RAMG {}, BANK1 {:05B}, BANK2 {:02b}, MODE {:1B}",
                  m_ram_enable, m_bank1, m_bank2, m_banking_mode_select);
}

void Cartridge::write_values(uint16_t address, uint8_t value) {
    // Actual RAM/ROM writes
    if (memmap::is_in(address, memmap::CartridgeRomFixedBank)) {
        uint32_t bank_number = 0;
        if (m_banking_mode_select == 1) {
            bank_number = static_cast<uint32_t>(m_bank2 << 5);
        }
        uint32_t address_in_rom = get_address_in_rom(address, bank_number);
        assert(address_in_rom < m_rom.size() && "Write ROM fixed bank out of bounds");
        m_rom[address_in_rom] = value;
        return;
    }
    if (memmap::is_in(address, memmap::CartridgeRomBankSwitchable)) {
        uint32_t bank_number = static_cast<uint32_t>((m_bank2 << 5) | m_bank1);
        uint32_t address_in_rom = get_address_in_rom(address, bank_number);
        assert(address_in_rom < m_rom.size() && "Write ROM switchable bank out of bounds");
        m_rom[address_in_rom] = value;
        return;
    }
    if (memmap::is_in(address, memmap::CartridgeRam)) {
        if (!m_ram_enable) {
            m_logger->warn("Write to {:04X} with disabled ram");
        }
        uint32_t bank_number = 0;
        if (m_banking_mode_select == 1) {
            bank_number = static_cast<uint32_t>(m_bank2 << 13);
        }
        uint32_t address_in_rom = bank_number << 13 | (address & 0b1111111111111);
        assert(address_in_rom < m_rom.size() && "Write to cartridge RAM bank out of bounds");
        m_rom[address_in_rom] = value;
    }
}
