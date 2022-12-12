#pragma once

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
#include <span>
#include <cstdint>


class Window {
    SDL_Window* m_sdl_window = nullptr;
    SDL_Renderer* m_sdl_renderer = nullptr;
    SDL_Texture* m_vram_texture = nullptr;
    bool m_done = false;
public:
    Window();
    ~Window();

    void draw_frame(std::span<uint8_t, 8192> vram);
    void draw_vram_viewer(std::span<uint8_t, 8192> vram);
    bool is_done() const;

};