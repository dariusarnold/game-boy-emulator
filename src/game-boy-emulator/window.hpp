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
    bool m_done = false;

    void handle_user_keyboard_input(const SDL_Event& event, std::shared_ptr<Joypad> joypad);

public:
    Window();
    ~Window();

    void draw_frame(const Emulator& emulator);
    void draw_tile_data_viewer(std::span<uint8_t, memmap::TileDataSize> vram);
    void draw_background(const Framebuffer<graphics::gb::ColorScreen>& background,
                         std::pair<uint8_t, uint8_t> viewport_position);
    void draw_window(std::vector<uint8_t> window);

    //    void draw_game(const std::vector<ScreenColor>&)

    bool is_done() const;
};