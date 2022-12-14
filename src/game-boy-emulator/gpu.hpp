#pragma once

#include "memorymap.hpp"
#include "ppu_registers.hpp"
class Emulator;
namespace spdlog {
class logger;
}
#include <array>
#include <span>
#include <memory>

struct OamEntry {
    uint8_t m_y_position;
    uint8_t m_x_position;
    uint8_t m_tile_index;
    uint8_t m_flags;
};
static_assert(sizeof(OamEntry) == 4 && "Padding in OamEntry detected");
static_assert(alignof(OamEntry) == 1 && "Invalid alignment of OamEntry detected");

class Gpu {
    std::array<uint8_t, memmap::TileDataSize> m_vram{};
    std::array<uint8_t, memmap::TileMapsSize> m_tile_maps{};
    std::array<OamEntry, memmap::OamRamSize / sizeof(OamEntry)> m_oam_ram{};
    PpuRegisters m_registers;
    std::shared_ptr<spdlog::logger> m_logger;
    Emulator* m_emulator;
    int m_clock_count = 0;


public:
    explicit Gpu(Emulator* emulator);

    uint8_t read_byte(uint16_t address);

    void write_byte(uint16_t address, uint8_t value);

    std::span<uint8_t, 8192> get_vram();
    void cycle_elapsed_callback(size_t cycles_m_num);
};
