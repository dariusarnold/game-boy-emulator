#pragma once

#include <SDL_video.h>
class SDL_Window;
#include <span>


class Window {
    SDL_GLContext m_gl_context;
    SDL_Window* m_sdl_window;
    bool m_done = false;
public:
    Window();
    ~Window();

    void draw_frame(std::span<uint8_t, 8192> vram);
    void draw_vram_viewer(std::span<uint8_t, 8192> vram);
    bool is_done() const;

};