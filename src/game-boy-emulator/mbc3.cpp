#include "mbc3.hpp"

#include "memorymap.hpp"
#include "exceptions.hpp"
#include "bitmanipulation.hpp"
#include "fmt/format.h"
#include "spdlog/spdlog.h"
#include "magic_enum.hpp"
#include <cassert>


uint8_t Mbc3::read_byte(uint16_t address) const {
    if (memmap::is_in(address, memmap::CartridgeRomFixedBank)) {
        // Bank 0 is fixed. Subtracting offset from address is not required since the bank zeroes
        // address range starts at 0.
        return get_rom()[address];
    }
    if (memmap::is_in(address, memmap::CartridgeRomBankSwitchable)) {
        const auto address_in_rom = address - memmap::CartridgeRomBankSwitchableBegin
                                    + m_rom_bank_number * memmap::CartridgeRomBankSwitchableSize;
        assert(address_in_rom < static_cast<int>(get_rom().size())
               && "Read ROM switchable bank out of bounds");
        return get_rom()[static_cast<size_t>(address_in_rom)];
    }
    if (memmap::is_in(address, memmap::CartridgeRam)) {
        if (m_ram_or_rtc_mapped == RamOrRtcMapped::RamMapped) {
            // Ram mapped, access it
            const auto address_in_ram = address - memmap::CartridgeRamBegin
                                        + m_ram_bank_number * memmap::CartridgeRamSize;
            assert(address_in_ram < static_cast<int>(get_ram().size())
                   && "Read RAM switchable bank out of bounds");
            return get_ram()[static_cast<size_t>(address_in_ram)];
        }
        // RTC mapped
        switch (m_current_rtc_register) {
        case RtcRegisterValue::RTC_S:
            return m_rtc.m_seconds;
        case RtcRegisterValue::RTC_M:
            return m_rtc.m_minutes;
        case RtcRegisterValue::RTC_H:
            return m_rtc.m_hours;
        case RtcRegisterValue::RTC_DL:
            return m_rtc.m_days_low;
        case RtcRegisterValue::RTC_DH:
            return m_rtc.m_days_high_and_flags;
        }
    }
    throw LogicError(fmt::format("Cartridge trying to read from {:04X}", address));
}

void Mbc3::write_byte(uint16_t address, uint8_t value) {
    write_registers(address, value);
    write_values(address, value);
}

void Mbc3::write_registers(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::RamEnable)) {
        // Ram and timer enable
        if (value == 0x0A) {
            m_ram_and_timer_enable = true;
        }
        if (value == 0x00) {
            m_ram_and_timer_enable = false;
        }
    } else if (memmap::is_in(address, memmap::RomBankNumber)) {
        // Select one of the rom banks. Rom bank 00 is not supported, trying to map it instead
        // accesses rom bank 01.
        if (value == 0) {
            value = 1;
        }
        m_rom_bank_number = (value & 0b1111111);
    } else if (memmap::is_in(address, memmap::RamBankNumber)) {
        // Ram bank number or RTC register select
        if (value <= 3) {
            // Select external ram bank for memory range A000-BFFF
            m_ram_bank_number = value & 0b11;
            m_ram_or_rtc_mapped = RamOrRtcMapped::RamMapped;
        } else {
            // Select rtc register for memory range A000-BFFF
            m_ram_or_rtc_mapped = RamOrRtcMapped::RtcMapped;
            if (magic_enum::enum_contains<Mbc3::RtcRegisterValue>(value)) {
                m_current_rtc_register = static_cast<Mbc3::RtcRegisterValue>(value);
            } else {
                get_logger()->warn("Ineffective write to cartridge register {:04X}", address);
            }
        }
    } else if (memmap::is_in(address, memmap::BankingModeSelect)) {
        // Latch clock data if previous write was 0x0 and this write was 0x1
        throw NotImplementedError("Unimplemented: Latch clock data");
    }
    //    get_logger()->debug("Cartridge registers: RAMG {}, BANK1 {:05B}, BANK2 {:02b}, MODE
    //    {:1B}",
    //                        m_ram_and_timer_enable, m_bank1, m_bank2, m_banking_mode_select);
}

void Mbc3::write_values(uint16_t address, uint8_t value) {
    // Actual RAM/RTC writes
    if (memmap::is_in(address, memmap::CartridgeRam)) {
        if (!m_ram_and_timer_enable) {
            get_logger()->warn("Write to {:04X} with disabled ram/timer");
            return;
        }
        if (m_ram_or_rtc_mapped == RamOrRtcMapped::RamMapped) {
            // Access RAM
            const auto address_in_ram = address - memmap::CartridgeRamBegin
                                        + m_ram_bank_number * memmap::CartridgeRamSize;
            assert(address_in_ram < static_cast<int>(get_ram().size())
                   && "Write to cartridge RAM bank out of bounds");
            get_ram()[static_cast<size_t>(address_in_ram)] = value;
        } else {
            // Access RTC
            switch (m_current_rtc_register) {
            case RtcRegisterValue::RTC_S:
                m_rtc.m_seconds = value;
                break;
            case RtcRegisterValue::RTC_M:
                m_rtc.m_minutes = value;
                break;
            case RtcRegisterValue::RTC_H:
                m_rtc.m_hours = value;
                break;
            case RtcRegisterValue::RTC_DL:
                m_rtc.m_days_low = value;
                break;
            case RtcRegisterValue::RTC_DH:
                m_rtc.m_days_high_and_flags = value;
                break;
            }
        }
    }
}
