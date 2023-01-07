#pragma once

#include "mbc.hpp"
#include <functional>

struct Rtc {
    uint8_t m_seconds = 0;
    uint8_t m_minutes = 0;
    uint8_t m_hours = 0;
    // Lower 8 bits of day counter
    uint8_t m_days_low = 0;
    // Upper 1 bit of day counter, halt and Day counter carry bit
    uint8_t m_days_high_and_flags = 0;
};

class Mbc3 : public Mbc {
    // Registers
    // RAM Enable (0x0000-0x1FFF)
    bool m_ram_and_timer_enable = false;
    // Only 7 bits
    uint8_t m_rom_bank_number = 0b00001;
    // Only 2 bits, (0x4000-0x5FFF)
    uint8_t m_ram_bank_number = 0;

    enum class RamOrRtcMapped {
        RamMapped,
        RtcMapped,
    };
    RamOrRtcMapped m_ram_or_rtc_mapped = RamOrRtcMapped::RamMapped;

    Rtc m_rtc{};

    // Value required to map the corresponding RTC register to A000-BFFF
    enum class RtcRegisterValue {
        RTC_S = 0x08,
        RTC_M = 0x09,
        RTC_H = 0x0A,
        RTC_DL = 0x0B,
        RTC_DH = 0x0C,
    };

    // Which register is currently mapped
    RtcRegisterValue m_current_rtc_register;

    void write_registers(uint16_t address, uint8_t value);
    void write_values(uint16_t address, uint8_t value);

public:
    using Mbc::Mbc;
    [[nodiscard]] uint8_t read_byte(uint16_t address) const override;
    void write_byte(uint16_t address, uint8_t value) override;
};
