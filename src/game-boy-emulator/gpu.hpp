#pragma once

#include "memorymap.hpp"
#include "ppu_registers.hpp"
#include "constants.h"
#include "framebuffer.hpp"
class Emulator;
namespace spdlog {
class logger;
}
#include <array>
#include <span>
#include <memory>
#include <vector>

struct OamEntry {
    uint8_t m_y_position;
    uint8_t m_x_position;
    uint8_t m_tile_index;
    uint8_t m_flags;
};
static_assert(sizeof(OamEntry) == 4 && "Padding in OamEntry detected");
static_assert(alignof(OamEntry) == 1 && "Invalid alignment of OamEntry detected");

class Gpu {
    // 0x8000-0x97FFF
    std::array<uint8_t, memmap::TileDataSize> m_tile_data{};
    // 0x9800-0x9FFF
    std::array<uint8_t, memmap::TileMapsSize> m_tile_maps{};
    // 0xFE00-0xFE9F
    std::array<OamEntry, memmap::OamRamSize / sizeof(OamEntry)> m_oam_ram{};
    static_assert(sizeof(m_oam_ram) == 40 * 4);
    PpuRegisters m_registers;
    std::shared_ptr<spdlog::logger> m_logger;
    Emulator* m_emulator;
    int m_clock_count = 0;

    void write_scanline();
    void write_sprites();
    void draw_window_line();
    void draw_background_line();

    // Framebuffers for the background. Use one in gameboy pixel format to allow easier reuse in
    // the rendering loop and one in screen pixel format to use with SDL/Dear ImGui.
    Framebuffer<graphics::gb::ColorGb> m_background_framebuffer_gb;
    Framebuffer<graphics::gb::ColorScreen> m_background_framebuffer_screen;

    Framebuffer<graphics::gb::ColorScreen> m_sprites_framebuffer_screen;

    std::span<uint8_t, 16> get_sprite_tile(uint8_t tile_index);

public:
    explicit Gpu(Emulator* emulator);

    uint8_t read_byte(uint16_t address);

    void write_byte(uint16_t address, uint8_t value);

    void cycle_elapsed_callback(size_t cycles_m_num);

    // Get one background or window tile from vram using the tile index.
    std::span<uint8_t, constants::BYTES_PER_TILE> get_tile(uint8_t tile_index);
    // Get one background or window tile from vram using tile coordinates (of 32x32)
    std::span<uint8_t, constants::BYTES_PER_TILE> get_tile_from_map(uint8_t tile_map_x, uint8_t tile_map_y);

    std::span<uint8_t, memmap::TileDataSize> get_vram_tile_data();

    const Framebuffer<graphics::gb::ColorScreen>& get_background();
    const Framebuffer<graphics::gb::ColorScreen>& get_sprites();
    std::vector<uint8_t> get_window();

    [[nodiscard]] std::pair<uint8_t, uint8_t> get_viewport_position() const;
};
