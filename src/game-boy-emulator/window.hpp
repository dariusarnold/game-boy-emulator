#pragma once

#include "memorymap.hpp"
#include "image.hpp"
class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
union SDL_Event;
class Emulator;
class Joypad;
#include <span>
#include <cstdint>
#include <vector>


class Window {
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
    void draw_background(std::vector<uint8_t> background,
                         std::pair<uint8_t, uint8_t> viewport_position);
    void draw_window(std::vector<uint8_t> window);

    bool is_done() const;
};