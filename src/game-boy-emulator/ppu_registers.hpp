#pragma once

#include "memorymap.hpp"
#include "graphics.hpp"
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
        ScxRegister = 0xFF43,
        LyRegister = 0xFF44,
        LycRegister = 0xFF45,
        DmaTransfer = 0xFF46,
        BgpRegister = 0xFF47,
        Obp0Register = 0xFF48,
        Obp1Register = 0xFF49,
        WyRegister = 0xFF4A,
        WxRegister = 0xFF4B,
    };


    // Values are the bits in the STAT register representing the STAT interrupt enable flag
    enum class StatInterruptSource {
        HBlank = 3,
        VBlank = 4,
        Oam = 5,
        LycEqualsLy = 6,
    };

    enum class BgWinAddressMode {
        /*
         * BG/Window tile IDs:
         * 0-127 in 8000-87FF
         * 128-255 in 8800-8FFF
         */
        Unsigned,
        /*
         * BG/Window tile IDs:
         * 0-127 in 9000-97FF
         * 128-255 in 8800-88FF
         */
        Signed,
    };

    enum class TileMapAddressRange {
        // Use tile map at 9800-9BFF
        Low,
        // Use tile map at 9C00-9FFF
        High,
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

    enum class LcdStatBits {
        LycEqualsLy = 2,
    };

    // Special helper function for querying state from the registers
    void set_mode(PpuMode mode);
    [[nodiscard]] PpuMode get_mode() const;
    [[nodiscard]] bool is_stat_interrupt_enabled(StatInterruptSource stat_interrupt) const;
    // Get bit 4 in LCDC register
    [[nodiscard]] BgWinAddressMode get_bg_win_address_mode() const;
    // Get bit 3 in LCDC register
    [[nodiscard]] TileMapAddressRange get_background_address_range() const;
    // Get bit 6 in LCDC register
    [[nodiscard]] TileMapAddressRange get_window_address_range() const;
    [[nodiscard]] bool is_ppu_enabled() const;
    [[nodiscard]] bool was_oam_transfer_requested() const;
    void clear_oam_transfer_request();
    [[nodiscard]] bool is_background_enabled() const;
    [[nodiscard]] bool is_window_enabled() const;
    [[nodiscard]] std::array<graphics::gb::ColorGb, 4> get_background_palette() const;

    // General helper functions for writing/reading from the registers
    [[nodiscard]] uint8_t get_register_value(PpuRegisters::Register r) const;
    [[nodiscard]] uint8_t get_register_value(uint16_t address) const;
    [[nodiscard]] uint8_t get_register_bit(PpuRegisters::Register r, uint8_t bit_position) const;
    void set_register_value(PpuRegisters::Register r, uint8_t value);
    void set_register_value(uint16_t address, uint8_t value);
    void set_register_bit(PpuRegisters::Register r, uint8_t bit_position, uint8_t bit_value);
    void increment_register(PpuRegisters::Register r);

private:
    uint8_t& get(PpuRegisters::Register r);
    [[nodiscard]] const uint8_t& get(PpuRegisters::Register r) const;
    bool m_fix_ly_register_value;
    bool m_oam_transfer_requested = false;
};
