#pragma once

#include "memorymap.hpp"
#include "image.hpp"
#include "graphics.hpp"
class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
union SDL_Event;
class Emulator;
class EmulatorState;
class Joypad;
namespace spdlog {
class logger;
}
#include "boost/circular_buffer.hpp"
#include <span>
#include <cstdint>
#include <vector>


class Window {
    Emulator& m_emulator;
    std::shared_ptr<spdlog::logger> m_logger;
    SDL_Window* m_sdl_window = nullptr;
    SDL_Renderer* m_sdl_renderer = nullptr;
    Image m_background_image;
    Image m_window_image;
    Image m_sprites_image;
    Image m_game_image;
    Image m_tiledata_block0;
    Image m_tiledata_block1;
    Image m_tiledata_block2;
    bool m_done = false;
    size_t m_previous_ticks = 0;

    std::array<bool, 8> m_pressed_keys{};
    boost::circular_buffer<float> m_fps_history;

    void handle_user_keyboard_input(const SDL_Event& event, std::shared_ptr<Joypad> joypad);

public:
    explicit Window(Emulator& emulator);
    ~Window();

    void draw_frame();
    void draw_background();
    void draw_sprites();
    void draw_window();
    void draw_game();
    void draw_info(const EmulatorState& state);
    void draw_vram();
    void draw_menubar();

    void vblank_callback();

    bool is_done() const;
};