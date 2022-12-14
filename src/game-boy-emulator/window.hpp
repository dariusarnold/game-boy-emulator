#pragma once

#include "memorymap.hpp"
class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
class Emulator;
#include <span>
#include <cstdint>
#include <vector>


class Window {
    SDL_Window* m_sdl_window = nullptr;
    SDL_Renderer* m_sdl_renderer = nullptr;
    SDL_Texture* m_tile_data_texture = nullptr;
    SDL_Texture* m_bg_texture = nullptr;
    bool m_done = false;
public:
    Window();
    ~Window();

    void draw_frame(const Emulator& emulator);
    void draw_tile_data_viewer(std::span<uint8_t, memmap::TileDataSize> vram);
    void draw_background(std::vector<uint8_t> background);
    bool is_done() const;

};