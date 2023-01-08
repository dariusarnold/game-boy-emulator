#include "mbc5.hpp"

#include "memorymap.hpp"
#include "bitmanipulation.hpp"
#include "exceptions.hpp"
#include "fmt/format.h"
#include "spdlog/spdlog.h"
#include <cassert>


Mbc5::Mbc5(std::vector<uint8_t> rom, std::span<uint8_t> ram) :
        Mbc(rom, ram),
        m_required_rom_bits(std::log2(get_rom_info().size_bytes)),
        m_required_ram_bits(std::log2(get_ram_info().size_bytes)) {}


namespace {
bool decide_ram_enable(uint8_t value) {
    return bitmanip::get_low_nibble(value) == 0x0A;
}
} // namespace

uint8_t Mbc5::read_byte(uint16_t address) const {
    if (memmap::is_in(address, memmap::CartridgeRomFixedBank)) {
        assert(address < get_rom().size() && "Read ROM fixed bank out of bounds");
        // Subtracting start not required because the address range starts at 0.
        return get_rom()[address];
    }
    if (memmap::is_in(address, memmap::CartridgeRomBankSwitchable)) {
        auto address_bank_begin = get_rom_bank_number() * memmap::CartridgeRomBankSwitchableSize;
        auto address_in_bank = address - memmap::CartridgeRomBankSwitchableBegin;
        auto address_rom = static_cast<size_t>(address_bank_begin + address_in_bank);
        address_rom = bitmanip::mask(address_rom, m_required_rom_bits);
        assert(address_rom < get_rom().size() && "Read to cartridge RAM bank out of bounds");
        return get_rom()[address_rom];
    }
    if (memmap::is_in(address, memmap::CartridgeRam)) {
        if (!m_ram_enable) {
            get_logger()->error("Read from disabled cartridge RAM in Mbc5");
            return 0xFF;
        }
        auto address_bank_begin = (m_ram_bank_number & 0x0F) * memmap::CartridgeRamSize;
        auto address_in_bank = address - memmap::CartridgeRamBegin;
        auto address_in_ram = static_cast<size_t>(address_bank_begin + address_in_bank);
        address_in_ram = bitmanip::mask(address_in_ram, m_required_ram_bits);
        assert(address_in_ram < get_ram().size() && "Read to cartridge RAM bank out of bounds");
        return get_ram()[address_in_ram];
    }
    throw LogicError(fmt::format("MBC5 trying to read from {:04X}", address));
}

void Mbc5::write_byte(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::CartridgeRam)) {
        write_values(address, value);
    } else {
        write_registers(address, value);
    }
}

namespace {
const size_t ROM_BANK_HIGH_BEGIN = 0x3000;
}

void Mbc5::write_registers(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::RamEnable)) {
        m_ram_enable = decide_ram_enable(value);
    } else if (memmap::is_in(address, memmap::RomBankNumber)) {
        if (address < ROM_BANK_HIGH_BEGIN) {
            m_rom_bank_number_low = value;
        } else {
            m_rom_bank_number_high = value & 1;
        }
    } else if (memmap::is_in(address, memmap::RamBankNumber)) {
        m_ram_bank_number = value;
    }
    get_logger()->debug("MBC5 registers: RAM enable {} RAM bank {} ROM_LOW {} ROM_HIGH {}",
                        m_ram_enable, m_ram_bank_number, m_rom_bank_number_low,
                        m_rom_bank_number_high);
}

void Mbc5::write_values(uint16_t address, uint8_t value) {
    // Actual RAM writes
    if (!m_ram_enable) {
        get_logger()->warn("Write to {:04X} with disabled ram", address);
        return;
    }

    auto address_bank_begin = (m_ram_bank_number & 0x0F) * memmap::CartridgeRamSize;
    auto address_in_bank = address - memmap::CartridgeRamBegin;
    auto address_in_ram = static_cast<size_t>(address_bank_begin + address_in_bank);
    address_in_ram = bitmanip::mask(address_in_ram, m_required_ram_bits);
    assert(address_in_ram < get_ram().size() && "Read to cartridge RAM bank out of bounds");
    get_ram()[address_in_ram] = value;
}

uint16_t Mbc5::get_rom_bank_number() const {
    return bitmanip::word_from_bytes(m_rom_bank_number_high & 1, m_rom_bank_number_low);
}
