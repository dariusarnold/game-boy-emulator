#pragma once

#include "memorymap.hpp"
#include "ppu_registers.hpp"
#include "constants.h"
#include "framebuffer.hpp"
#include "dmatransfer.hpp"
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

class Ppu {
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
    // Framebuffer for the game
    Framebuffer<graphics::gb::ColorScreen, constants::SCREEN_RES_WIDTH,
                constants::SCREEN_RES_HEIGHT>
        m_game_framebuffer;
    // Framebuffers for debug elements
    Framebuffer<graphics::gb::ColorScreen, constants::BACKGROUND_SIZE_PIXELS,
                constants::BACKGROUND_SIZE_PIXELS>
        m_background_framebuffer;
    Framebuffer<graphics::gb::ColorScreen, constants::SCREEN_RES_WIDTH,
                constants::SCREEN_RES_HEIGHT>
        m_sprites_framebuffer;
    Framebuffer<graphics::gb::ColorScreen, constants::BACKGROUND_SIZE_PIXELS,
                constants::BACKGROUND_SIZE_PIXELS>
        m_window_framebuffer;
    Framebuffer<graphics::gb::ColorScreen,
                constants::SPRITE_VIEWER_WIDTH * constants::PIXELS_PER_TILE,
                constants::SPRITE_VIEWER_HEIGHT * constants::PIXELS_PER_TILE>
        m_tiledata_block0;
    Framebuffer<graphics::gb::ColorScreen,
                constants::SPRITE_VIEWER_WIDTH * constants::PIXELS_PER_TILE,
                constants::SPRITE_VIEWER_HEIGHT * constants::PIXELS_PER_TILE>
        m_tiledata_block1;
    Framebuffer<graphics::gb::ColorScreen,
                constants::SPRITE_VIEWER_WIDTH * constants::PIXELS_PER_TILE,
                constants::SPRITE_VIEWER_HEIGHT * constants::PIXELS_PER_TILE>
        m_tiledata_block2;
    OamDmaTransfer m_oam_dma_transfer;

    void write_scanline();
    void write_sprites(Framebuffer<graphics::gb::ColorScreen, constants::SCREEN_RES_WIDTH,
                                   constants::SCREEN_RES_HEIGHT>& framebuffer);
    void draw_sprites();
    void draw_sprites_debug();
    void draw_window_line();
    void draw_background_line();
    void draw_background_debug();
    void draw_window_debug();
    void draw_vram_debug();

    std::span<uint8_t, 16> get_sprite_tile(uint8_t tile_index);
    std::span<uint8_t, 16> get_tile(unsigned block, unsigned index_in_block);

    enum class TileType { Background, Window };
    // Get one background or window tile from vram using the tile index.
    std::span<uint8_t, constants::BYTES_PER_TILE> get_tile(uint8_t tile_index);
    // Get one background or window tile from vram using tile coordinates (of 32x32)
    std::span<uint8_t, constants::BYTES_PER_TILE>
    get_tile_from_map(TileType tile_type, uint8_t tile_map_x, uint8_t tile_map_y);

    void start_oam_dma_transfer();

public:
    explicit Ppu(Emulator* emulator);

    uint8_t read_byte(uint16_t address);

    void write_byte(uint16_t address, uint8_t value);

    void cycle_elapsed_callback(size_t cycles_m_num);

    std::span<uint8_t, memmap::TileDataSize> get_vram_tile_data();

    const auto& get_game() {
        return m_game_framebuffer;
    }
    const auto& get_background() {
        return m_background_framebuffer;
    }
    const auto& get_sprites() {
        return m_sprites_framebuffer;
    }
    const auto& get_window() {
        return m_window_framebuffer;
    }
    const std::array<
        const Framebuffer<graphics::gb::ColorScreen,
                          constants::SPRITE_VIEWER_WIDTH * constants::PIXELS_PER_TILE,
                          constants::SPRITE_VIEWER_HEIGHT * constants::PIXELS_PER_TILE>*,
        3>
    get_tiledata();

    [[nodiscard]] std::pair<uint8_t, uint8_t> get_viewport_position() const;
};
