#pragma once

#include "memorymap.hpp"
#include "image.hpp"
#include "graphics.hpp"
class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
union SDL_Event;
class Emulator;
class Joypad;
namespace spdlog {
class logger;
}
#include <span>
#include <cstdint>
#include <vector>


class Window {
    std::shared_ptr<spdlog::logger> m_logger;
    SDL_Window* m_sdl_window = nullptr;
    SDL_Renderer* m_sdl_renderer = nullptr;
    Image m_tile_data_image;
    Image m_background_image;
    Image m_window_image;
    Image m_sprites_image;
    Image m_game_image;
    bool m_done = false;

    void handle_user_keyboard_input(const SDL_Event& event, std::shared_ptr<Joypad> joypad);

public:
    Window();
    ~Window();

    void draw_frame(const Emulator& emulator);
    void draw_tile_data_viewer(std::span<uint8_t, memmap::TileDataSize> vram);
    void draw_background(const Framebuffer<graphics::gb::ColorScreen>& background,
                         std::pair<uint8_t, uint8_t> viewport_position);
    void draw_sprites(const Framebuffer<graphics::gb::ColorScreen>& sprites);
    void draw_window(const Framebuffer<graphics::gb::ColorScreen>& window);
    void draw_game();

    void vblank_callback(const Framebuffer<graphics::gb::ColorScreen>& game);

    bool is_done() const;
};