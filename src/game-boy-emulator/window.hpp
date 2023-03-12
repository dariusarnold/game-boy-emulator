#pragma once

#include "memorymap.hpp"
#include "image.hpp"
#include "graphics.hpp"
#include "constants.h"
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
    Image<constants::BACKGROUND_SIZE_PIXELS, constants::BACKGROUND_SIZE_PIXELS> m_background_image;
    Image<constants::BACKGROUND_SIZE_PIXELS, constants::BACKGROUND_SIZE_PIXELS> m_window_image;
    Image<constants::SCREEN_RES_WIDTH, constants::SCREEN_RES_HEIGHT> m_sprites_image;
    Image<constants::SCREEN_RES_WIDTH, constants::SCREEN_RES_HEIGHT> m_game_image;
    Image<constants::SPRITE_VIEWER_WIDTH * constants::PIXELS_PER_TILE,
          constants::SPRITE_VIEWER_HEIGHT * constants::PIXELS_PER_TILE>
        m_tiledata_block0;
    Image<constants::SPRITE_VIEWER_WIDTH * constants::PIXELS_PER_TILE,
          constants::SPRITE_VIEWER_HEIGHT * constants::PIXELS_PER_TILE>
        m_tiledata_block1;
    Image<constants::SPRITE_VIEWER_WIDTH * constants::PIXELS_PER_TILE,
          constants::SPRITE_VIEWER_HEIGHT * constants::PIXELS_PER_TILE>
        m_tiledata_block2;
    bool m_done = false;
    size_t m_previous_ticks = 0;

    std::array<bool, 8> m_pressed_keys{};
    boost::circular_buffer<float> m_fps_history;

    void handle_user_keyboard_input(const SDL_Event& event, const std::shared_ptr<Joypad>& joypad);

    void draw_menubar_file();
    void draw_menubar_settings();

    void draw_background();
    void draw_sprites();
    void draw_window();
    void draw_game();
    void draw_info();
    void draw_vram();
    void draw_menubar();
    void draw_frame();

public:
    explicit Window(Emulator& emulator);
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;
    ~Window();

    void vblank_callback();

    [[nodiscard]] bool is_done() const;
};