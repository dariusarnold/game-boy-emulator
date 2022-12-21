#include "gpu.hpp"
#include "exceptions.hpp"
#include "memorymap.hpp"
#include "emulator.hpp"
#include "interrupthandler.hpp"
#include "addressbus.hpp"
#include "bitmanipulation.hpp"
#include "graphics.hpp"

#include "fmt/format.h"
#include "spdlog/spdlog.h"
#include "magic_enum.hpp"

#include <span>

Gpu::Gpu(Emulator* emulator) :
        m_registers(emulator->get_options().stub_ly),
        m_logger(spdlog::get("")),
        m_emulator(emulator),
        m_background_framebuffer_screen(constants::BACKGROUND_SIZE_PIXELS,
                                        constants::BACKGROUND_SIZE_PIXELS,
                                        graphics::gb::ColorScreen::White),
        m_sprites_framebuffer_screen(constants::VIEWPORT_WIDTH, constants::VIEWPORT_HEIGHT),
        m_window_framebuffer_screen(constants::BACKGROUND_SIZE_PIXELS,
                                    constants::BACKGROUND_SIZE_PIXELS,
                                    graphics::gb::ColorScreen::White),
        m_game_framebuffer_screen(constants::VIEWPORT_WIDTH, constants::VIEWPORT_HEIGHT,
                                  graphics::gb::ColorScreen::White) {}

uint8_t Gpu::read_byte(uint16_t address) {
    if (memmap::is_in(address, memmap::TileData)) {
        if (m_registers.is_ppu_enabled() && m_registers.get_mode() == PpuMode::PixelTransfer_3) {
            m_logger->error("GPU: VRAM read at {:04X} during pixel transfer", address);
        }
        return m_tile_data[address - memmap::VRamBegin];
    }
    if (memmap::is_in(address, memmap::OamRam)) {
        const auto mode = m_registers.get_mode();
        if (m_registers.is_ppu_enabled()
            && (mode == PpuMode::OamScan_2 || mode == PpuMode::PixelTransfer_3)) {
            m_logger->error("GPU: OAM read at {:04X} during mode {}", address,
                            magic_enum::enum_name(mode));
        }
        return reinterpret_cast<uint8_t*>(m_oam_ram.data())[address - memmap::OamRamBegin];
    }
    if (memmap::is_in(address, memmap::PpuIoRegisters)) {
        return m_registers.get_register_value(address);
    }
    if (memmap::is_in(address, memmap::TileMaps)) {
        return m_tile_maps[address - memmap::TileMapsBegin];
    }

    throw LogicError(fmt::format("GPU can't read from {:04X}", address));
}

void Gpu::write_byte(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::TileData)) {
        if (m_registers.is_ppu_enabled() && m_registers.get_mode() == PpuMode::PixelTransfer_3) {
            m_logger->error("GPU: VRAM write at {:04X} during pixel transfer", address);
        }
        m_tile_data[address - memmap::VRamBegin] = value;
    } else if (memmap::is_in(address, memmap::PpuIoRegisters)) {
        m_registers.set_register_value(address, value);
    } else if (memmap::is_in(address, memmap::OamRam)) {
        if (m_registers.is_ppu_enabled()
            && (m_registers.get_mode() == PpuMode::OamScan_2
                || m_registers.get_mode() == PpuMode::PixelTransfer_3)) {
            m_logger->error("GPU: Oam write at {:04X} during mode {}", address,
                            magic_enum::enum_name(m_registers.get_mode()));
        }
        reinterpret_cast<uint8_t*>(m_oam_ram.data())[address - memmap::OamRamBegin] = value;
    } else if (memmap::is_in(address, memmap::TileMaps)) {
        if (m_registers.is_ppu_enabled() && m_registers.get_mode() == PpuMode::PixelTransfer_3) {
            m_logger->error("GPU: VRAM write at {:04X} during pixel transfer", address);
        }
        m_tile_maps[address - memmap::TileMapsBegin] = value;
    } else {
        m_logger->error("GPU: unhandled write to {:04X}", address);
    }
}

std::span<uint8_t, memmap::TileDataSize> Gpu::get_vram_tile_data() {
    return {m_tile_data};
}

namespace {
// in M cycles
const int DURATION_OAM_SEARCH = 20;
const int DURATION_PIXEL_TRANSFER = 43;
const int DURATION_H_BLANK = 51;
const int DURATION_SCANLINE = DURATION_OAM_SEARCH + DURATION_PIXEL_TRANSFER + DURATION_H_BLANK;
// const int DURATION_V_BLANK = 10 * (20 + 43 + 51);
} // namespace

void Gpu::cycle_elapsed_callback(size_t cycles_m_num) {
    if (m_registers.was_oam_transfer_requested()) {
        m_registers.clear_oam_transfer_request();
        auto high_byte_address
            = m_registers.get_register_value(PpuRegisters::Register::DmaTransfer);
        auto start_address = bitmanip::word_from_bytes(high_byte_address, 0);
        m_logger->info("OAM DMA transfer from {:04X}", start_address);
        for (int i = 0; i < memmap::OamRamSize; ++i) {
            auto x = m_emulator->get_bus()->read_byte(start_address + i);
            reinterpret_cast<uint8_t*>(m_oam_ram.data())[i] = x;
        }
    }

    (void)cycles_m_num;
    auto mode = m_registers.get_mode();
    m_clock_count++;
    switch (mode) {
    case PpuMode::OamScan_2:
        if (m_clock_count >= DURATION_OAM_SEARCH) {
            m_logger->info("GPU: cycle {}, LY {}, mode {}->{}", m_clock_count,
                           m_registers.get_register_value(PpuRegisters::Register::LyRegister),
                           static_cast<int>(m_registers.get_mode()),
                           static_cast<int>(PpuMode::PixelTransfer_3));

            m_clock_count = m_clock_count % DURATION_OAM_SEARCH;
            m_registers.set_mode(PpuMode::PixelTransfer_3);
        }
        break;
    case PpuMode::PixelTransfer_3:
        if (m_clock_count >= DURATION_PIXEL_TRANSFER) {
            m_logger->info("GPU: cycle {}, LY {}, mode {}->{}", m_clock_count,
                           m_registers.get_register_value(PpuRegisters::Register::LyRegister),
                           static_cast<int>(m_registers.get_mode()),
                           static_cast<int>(PpuMode::HBlank_0));

            m_clock_count = m_clock_count % DURATION_PIXEL_TRANSFER;

            if (m_registers.is_stat_interrupt_enabled(PpuRegisters::StatInterruptSource::HBlank)) {
                m_emulator->get_interrupt_handler()->request_interrupt(
                    InterruptHandler::InterruptType::LcdStat);
            }

            auto ly_equals_lyc
                = m_registers.get_register_value(PpuRegisters::Register::LyRegister)
                  == m_registers.get_register_value(PpuRegisters::Register::LycRegister);
            if (m_registers.is_stat_interrupt_enabled(
                    PpuRegisters::StatInterruptSource::LycEqualsLy)
                && ly_equals_lyc) {
                m_emulator->get_interrupt_handler()->request_interrupt(
                    InterruptHandler::InterruptType::LcdStat);
            }
            m_registers.set_register_bit(PpuRegisters::Register::StatRegister,
                                         static_cast<int>(PpuRegisters::LcdStatBits::LycEqualsLy),
                                         static_cast<int>(ly_equals_lyc));
            m_registers.set_mode(PpuMode::HBlank_0);
        }
        break;
    case PpuMode::HBlank_0:
        if (m_clock_count >= DURATION_H_BLANK) {

            write_scanline();
            m_registers.increment_register(PpuRegisters::Register::LyRegister);

            m_clock_count = m_clock_count % DURATION_H_BLANK;

            PpuMode new_mode{};
            // If we are on the last visible line we enter the vblank mode
            if (m_registers.get_register_value(PpuRegisters::Register::LyRegister) == 144) {
                new_mode = PpuMode::VBlank_1;
                m_registers.set_mode(new_mode);
                m_emulator->get_interrupt_handler()->request_interrupt(
                    InterruptHandler::InterruptType::VBlank);
            } else {
                new_mode = PpuMode::OamScan_2;
                m_registers.set_mode(new_mode);
            }

            m_logger->info("GPU: cycle {}, LY {}, mode {}->{}", m_clock_count,
                           m_registers.get_register_value(PpuRegisters::Register::LyRegister),
                           static_cast<int>(m_registers.get_mode()), static_cast<int>(new_mode));
        }
        break;
    case PpuMode::VBlank_1:
        if (m_clock_count >= DURATION_SCANLINE) {
            // Vblank duration is 10 scanlines
            m_registers.increment_register(PpuRegisters::Register::LyRegister);
            m_logger->info("GPU: cycle {}, LY {}, mode {}", m_clock_count,
                           m_registers.get_register_value(PpuRegisters::Register::LyRegister),
                           static_cast<int>(m_registers.get_mode()));
            m_clock_count = m_clock_count % DURATION_SCANLINE;

            if (m_registers.get_register_value(PpuRegisters::Register::LyRegister) == 154) {
                write_sprites();

                m_emulator->draw(m_game_framebuffer_screen);
                m_game_framebuffer_screen.reset();

                m_registers.set_register_value(PpuRegisters::Register::LyRegister, 0);
                m_logger->info("GPU: cycle {}, LY {}, mode {}->{}", m_clock_count,
                               m_registers.get_register_value(PpuRegisters::Register::LyRegister),
                               static_cast<int>(m_registers.get_mode()),
                               static_cast<int>(PpuMode::OamScan_2));
                m_registers.set_mode(PpuMode::OamScan_2);
            }
        }
        break;
    }
}

std::span<uint8_t, constants::BYTES_PER_TILE> Gpu::get_tile(uint8_t tile_index) {
    if (m_registers.get_bg_win_address_mode() == PpuRegisters::BgWinAddressMode::Unsigned) {
        // In unsigned indexing, the sprites are in order in memory, starting at 0x8000
        const size_t index_begin = tile_index * constants::BYTES_PER_TILE;
        return std::span<uint8_t, constants::BYTES_PER_TILE>{m_tile_data.data() + index_begin,
                                                             constants::BYTES_PER_TILE};
    }
    // In signed indexing, the first 0-127 tiles are from 0x9000-0x97FF (last block of the tile
    // data portion of the vram). The tiles 128-255 are in the second block from 0x8800-0x8FFF.
    if (static_cast<int8_t>(tile_index) > 0) {
        // Skip the first 256 sprites (block 0 and 1) and index into block 2.
        size_t const index_begin
            = 256 * constants::BYTES_PER_TILE + tile_index * constants::BYTES_PER_TILE;
        return std::span<uint8_t, constants::BYTES_PER_TILE>{m_tile_data.data() + index_begin,
                                                             constants::BYTES_PER_TILE};
    }
    // Tiles 128-255 lie within block 1. We can use the index from 0;
    size_t const index_begin = tile_index * constants::BYTES_PER_TILE;
    return std::span<uint8_t, constants::BYTES_PER_TILE>{m_tile_data.data() + index_begin,
                                                         constants::BYTES_PER_TILE};
}

std::span<uint8_t, constants::BYTES_PER_TILE>
Gpu::get_tile_from_map(TileType tile_type, uint8_t tile_map_x, uint8_t tile_map_y) {
    unsigned address_offset = 0;
    switch (tile_type) {
    case TileType::Background:
        if (m_registers.get_background_address_range() == PpuRegisters::TileMapAddressRange::High) {
            address_offset = memmap::TileMap1Size;
        }
        break;
    case TileType::Window:
        if (m_registers.get_window_address_range() == PpuRegisters::TileMapAddressRange::High) {
            address_offset = memmap::TileMap1Size;
        }
        break;
    }
    auto tile_map_index = address_offset + tile_map_x + tile_map_y * 32;
    auto tile_index = m_tile_maps[tile_map_index];
    return get_tile(tile_index);
}

const Framebuffer<graphics::gb::ColorScreen>& Gpu::get_background() {
    return m_background_framebuffer_screen;
}

const Framebuffer<graphics::gb::ColorScreen>& Gpu::get_window() {
    return m_window_framebuffer_screen;
}

void Gpu::write_scanline() {
    if (!m_registers.is_ppu_enabled()) {
        return;
    }
    if (m_registers.is_background_enabled()) {
        draw_background_line();
    }
    if (m_registers.is_window_enabled()) {
        draw_window_line();
    }
}

void Gpu::write_sprites() {
    m_sprites_framebuffer_screen.reset(graphics::gb::ColorScreen::White);
    if (m_registers.get_sprite_height() == 16) {
        throw LogicError("Tall sprites not supported");
    }

    for (const auto& oam_entry : m_oam_ram) {
        auto sprite_data = get_sprite_tile(oam_entry.m_tile_index);
        auto tile = graphics::gb::tile_to_gb_color(sprite_data);
        auto palette_bit = bitmanip::is_bit_set(oam_entry.m_flags, 4);
        auto palette = m_registers.get_obj0_palette();
        if (palette_bit == 1) {
            palette = m_registers.get_obj1_palette();
        }

        for (unsigned sprite_x = 0; sprite_x < 8; ++sprite_x) {
            for (unsigned sprite_y = 0; sprite_y < 8; ++sprite_y) {
                auto x = static_cast<int>(oam_entry.m_x_position + sprite_x) - 8;
                auto y = static_cast<int>(oam_entry.m_y_position + sprite_y) - 16;
                if (x < 0 || y < 0) {
                    // This pixel of the sprite is hidden
                    continue;
                }
                auto mapped_color
                    = palette[magic_enum::enum_integer(tile[sprite_x + sprite_y * 8])];
                auto color = graphics::gb::to_screen_color(mapped_color);
                m_sprites_framebuffer_screen.set_pixel(static_cast<size_t>(x),
                                                       static_cast<size_t>(y), color);
            }
        }
    }
}

void Gpu::draw_window_line() {
    unsigned const screen_y = m_registers.get_register_value(PpuRegisters::Register::LyRegister);
    unsigned const tile_y = screen_y / 8;

    // First update the line in the complete window framebuffer
    for (unsigned tile_x = 0; tile_x < 32; ++tile_x) {
        // Get tile by reading index from tile map and fetching indexed tile from tile data.
        auto tile = get_tile_from_map(TileType::Window, tile_x, tile_y);
        auto in_tile_y = screen_y % 8;
        // The tile provides an 8 pixel line from 2 bytes
        auto tile_line
            = graphics::gb::convert_tile_line(tile[in_tile_y * 2], tile[in_tile_y * 2 + 1]);
        auto palette = m_registers.get_background_palette();
        // Map the colors using the current palette and transfer this tiles line to the buffer
        for (unsigned tile_line_x = 0; tile_line_x < 8; tile_line_x++) {
            auto pixel = palette[magic_enum::enum_integer(tile_line[tile_line_x])];
            auto screen_x = tile_x * 8 + tile_line_x;
            m_window_framebuffer_screen.set_pixel(screen_x, screen_y,
                                                  graphics::gb::to_screen_color(pixel));
        }
    }
}

void Gpu::draw_background_line() {
    unsigned const screen_y = m_registers.get_register_value(PpuRegisters::Register::LyRegister);
    // Tile index
    unsigned const tile_y = screen_y / 8;
    // Pixel offset in tile
    auto in_tile_y = screen_y % 8;
    auto palette = m_registers.get_background_palette();

    // First update the line in the complete background framebuffer
    for (unsigned tile_x = 0; tile_x < 32; ++tile_x) {
        // Get tile by reading index from tile map and fetching indexed tile from tile data.
        auto tile = get_tile_from_map(TileType::Background, tile_x, tile_y);
        // The tile provides an 8 pixel line from 2 bytes
        auto tile_line
            = graphics::gb::convert_tile_line(tile[in_tile_y * 2], tile[in_tile_y * 2 + 1]);
        // Map the colors using the current palette and transfer this tiles line to the buffer
        for (unsigned tile_line_x = 0; tile_line_x < 8; tile_line_x++) {
            auto pixel = palette[magic_enum::enum_integer(tile_line[tile_line_x])];
            auto screen_x = tile_x * 8 + tile_line_x;
            m_background_framebuffer_screen.set_pixel(screen_x, screen_y,
                                                      graphics::gb::to_screen_color(pixel));
        }
    }

    // Then transfer only the scrolled part of the line to the game framebuffer
    auto scx = m_registers.get_register_value(PpuRegisters::Register::ScxRegister);
    auto scy = m_registers.get_register_value(PpuRegisters::Register::ScyRegister);

    for (unsigned screen_x = 0; screen_x < constants::SCREEN_RES_WIDTH; screen_x++) {
        auto scrolled_x = (screen_x + scx) % m_background_framebuffer_screen.width();
        auto scrolled_y = (screen_y + scy) % m_background_framebuffer_screen.height();
        auto pixel = m_background_framebuffer_screen.get_pixel(scrolled_x, scrolled_y);
        m_game_framebuffer_screen.set_pixel(screen_x, screen_y, pixel);
    }
}

std::pair<uint8_t, uint8_t> Gpu::get_viewport_position() const {
    auto x = m_registers.get_register_value(PpuRegisters::Register::ScxRegister);
    auto y = m_registers.get_register_value(PpuRegisters::Register::ScyRegister);
    return {x, y};
}

const Framebuffer<graphics::gb::ColorScreen>& Gpu::get_sprites() {
    return m_sprites_framebuffer_screen;
}

std::span<uint8_t, 16> Gpu::get_sprite_tile(uint8_t tile_index) {
    return std::span<uint8_t, 16>{m_tile_data.begin() + tile_index * constants::BYTES_PER_TILE,
                                  constants::BYTES_PER_TILE};
}
