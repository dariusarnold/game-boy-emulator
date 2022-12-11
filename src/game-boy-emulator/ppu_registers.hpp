#pragma once

#include "memorymap.hpp"
#include <cstdint>
#include <array>

class PpuRegisters {
    std::array<uint8_t, memmap::PpuIoRegistersSize> m_registers{};

public:
    explicit PpuRegisters(bool fix_ly);

    enum class Register : uint16_t {
        LcdcRegister = 0xFF40,
        StatRegister = 0xFF41,
        ScyRegister = 0xFF42,
        ScxRegister = 0xFF42,
        LyRegister = 0xFF44,
        LycRegister = 0xFF45,
        DmaTransfer = 0xFF46,
        BgpRegister = 0xFF47,
        Obp0Register = 0xFF48,
        Obp1Register = 0xFF49,
        WyRegister = 0xFF4A,
        WxRegister = 0xFF4B,
    };



    // General helpe functions for writing/reading from the registers
    uint8_t get_register_value(PpuRegisters::Register r);
    uint8_t get_register_value(uint16_t address);
    void set_register_value(PpuRegisters::Register r, uint8_t value);
    void set_register_value(uint16_t, uint8_t value);

private:
    uint8_t& get(PpuRegisters::Register r);
    [[nodiscard]] const uint8_t& get(PpuRegisters::Register r) const;

};
