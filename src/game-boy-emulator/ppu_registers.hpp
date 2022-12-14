#pragma once

#include "memorymap.hpp"
#include <cstdint>
#include <array>

enum class PpuMode {
    HBlank_0 = 0,
    VBlank_1 = 1,
    OamScan_2 = 2,
    PixelTransfer_3 = 3,
};

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


    enum class LcdcBits {
        BgWindowEnablePriority = 0,
        ObjEnable = 1,
        ObjSize = 2,
        BgTileMapArea = 3,
        BgWindowTileDataArea = 4,
        WindowEnable = 5,
        WindowTileMapArea = 6,
        LcdAndPpuEnable = 7,
    };


    // Special helper function for querying state from the registers
    void set_mode(PpuMode mode);
    [[nodiscard]] PpuMode get_mode() const;
    [[nodiscard]] bool is_ppu_enabled() const;

    // General helper functions for writing/reading from the registers
    uint8_t get_register_value(PpuRegisters::Register r) const;
    uint8_t get_register_value(uint16_t address) const;
    uint8_t get_register_bit(PpuRegisters::Register r, uint8_t bit_position) const;
    void set_register_value(PpuRegisters::Register r, uint8_t value);
    void set_register_value(uint16_t, uint8_t value);
    void set_register_bit(PpuRegisters::Register r, uint8_t bit_position, uint8_t bit_value);
    void increment_register(PpuRegisters::Register r);

private:
    uint8_t& get(PpuRegisters::Register r);
    [[nodiscard]] const uint8_t& get(PpuRegisters::Register r) const;
    bool m_fix_ly_register_value;

};
