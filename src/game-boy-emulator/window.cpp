#include "window.hpp"
#include "graphics.hpp"
#include "emulator.hpp"
#include "ppu.hpp"
#include "constants.h"
#include "joypad.hpp"

#include "spdlog/spdlog.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "SDL.h"

Window::Window(Emulator& emulator) :
        m_emulator(emulator),
        m_logger(spdlog::get("")),
        m_background_image(constants::BACKGROUND_SIZE_PIXELS, constants::BACKGROUND_SIZE_PIXELS),
        m_window_image(constants::BACKGROUND_SIZE_PIXELS, constants::BACKGROUND_SIZE_PIXELS),
        m_sprites_image(constants::SCREEN_RES_WIDTH, constants::SCREEN_RES_HEIGHT),
        m_game_image(constants::SCREEN_RES_WIDTH, constants::SCREEN_RES_HEIGHT),
        m_tiledata_block0(constants::SPRITE_VIEWER_WIDTH * constants::PIXELS_PER_TILE,
                          constants::SPRITE_VIEWER_HEIGHT * constants::PIXELS_PER_TILE),
        m_tiledata_block1(constants::SPRITE_VIEWER_WIDTH * constants::PIXELS_PER_TILE,
                          constants::SPRITE_VIEWER_HEIGHT * constants::PIXELS_PER_TILE),
        m_tiledata_block2(constants::SPRITE_VIEWER_WIDTH * constants::PIXELS_PER_TILE,
                          constants::SPRITE_VIEWER_HEIGHT * constants::PIXELS_PER_TILE) {
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a
    // minority of Windows systems, depending on whether SDL_INIT_GAMECONTROLLER is enabled or
    // disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        spdlog::error("Error: {}", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }

    auto window_flags
        = static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    m_sdl_window = SDL_CreateWindow("game boy emulator", SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, 1280, 720, window_flags);
    m_sdl_renderer = SDL_CreateRenderer(m_sdl_window, -1,
                                        SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(m_sdl_window, m_sdl_renderer);
    ImGui_ImplSDLRenderer_Init(m_sdl_renderer);

    m_background_image.init_texture(m_sdl_renderer);
    m_window_image.init_texture(m_sdl_renderer);
    m_sprites_image.init_texture(m_sdl_renderer);
    m_game_image.init_texture(m_sdl_renderer);
    m_tiledata_block0.init_texture(m_sdl_renderer);
    m_tiledata_block1.init_texture(m_sdl_renderer);
    m_tiledata_block2.init_texture(m_sdl_renderer);
}

Window::~Window() {
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(m_sdl_renderer);
    SDL_DestroyWindow(m_sdl_window);
    SDL_Quit();
}

namespace {
// TODO make the key bindings configurable
constexpr SDL_KeyCode KEY_UP = SDLK_w;
constexpr SDL_Keycode KEY_LEFT = SDLK_a;
constexpr SDL_Keycode KEY_DOWN = SDLK_s;
constexpr SDL_Keycode KEY_RIGHT = SDLK_d;
constexpr SDL_Keycode KEY_A = SDLK_j;
constexpr SDL_Keycode KEY_B = SDLK_k;
constexpr SDL_Keycode KEY_START = SDLK_n;
constexpr SDL_Keycode KEY_SELECT = SDLK_l;
} // namespace

void Window::draw_frame() {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui
    // wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main
    // application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main
    // application. Generally you may always pass all inputs to dear imgui, and hide them from
    // your application based on those two flags.
    SDL_Event event;
    while (SDL_PollEvent(&event) == 1) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) {
            m_done = true;
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE
            && event.window.windowID == SDL_GetWindowID(m_sdl_window)) {
            m_done = true;
        }
        handle_user_keyboard_input(event, m_emulator.get_joypad());
    }

    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    draw_background(m_emulator.get_ppu()->get_background(),
                    m_emulator.get_ppu()->get_viewport_position());
    draw_window(m_emulator.get_ppu()->get_window());
    draw_sprites(m_emulator.get_ppu()->get_sprites());
    draw_game();
    draw_info(m_emulator.get_state());
    draw_vram();

    // Rendering
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGui::Render();
    SDL_SetRenderDrawColor(m_sdl_renderer, (Uint8)(clear_color.x * 255),
                           (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255),
                           (Uint8)(clear_color.w * 255));
    SDL_RenderClear(m_sdl_renderer);
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(m_sdl_renderer);
}

void Window::handle_user_keyboard_input(const SDL_Event& event, std::shared_ptr<Joypad> joypad) {
    auto& io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard) {
        // Ignore repeated down events since they don't alter joypad state
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
            switch (event.key.keysym.sym) {
            case KEY_UP:
                joypad->press_key(Joypad::Keys::Up);
                m_pressed_keys[static_cast<int>(Joypad::Keys::Up)] = true;
                break;
            case KEY_LEFT:
                joypad->press_key(Joypad::Keys::Left);
                m_pressed_keys[static_cast<int>(Joypad::Keys::Left)] = true;
                break;
            case KEY_DOWN:
                joypad->press_key(Joypad::Keys::Down);
                m_pressed_keys[static_cast<int>(Joypad::Keys::Down)] = true;
                break;
            case KEY_RIGHT:
                joypad->press_key(Joypad::Keys::Right);
                m_pressed_keys[static_cast<int>(Joypad::Keys::Right)] = true;
                break;
            case KEY_A:
                joypad->press_key(Joypad::Keys::A);
                m_pressed_keys[static_cast<int>(Joypad::Keys::A)] = true;
                break;
            case KEY_B:
                joypad->press_key(Joypad::Keys::B);
                m_pressed_keys[static_cast<int>(Joypad::Keys::B)] = true;
                break;
            case KEY_START:
                joypad->press_key(Joypad::Keys::Start);
                m_pressed_keys[static_cast<int>(Joypad::Keys::Start)] = true;
                break;
            case KEY_SELECT:
                joypad->press_key(Joypad::Keys::Select);
                m_pressed_keys[static_cast<int>(Joypad::Keys::Select)] = true;
                break;
            }
        }
        if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
            case KEY_UP:
                joypad->release_key(Joypad::Keys::Up);
                m_pressed_keys[static_cast<int>(Joypad::Keys::Up)] = false;
                break;
            case KEY_LEFT:
                joypad->release_key(Joypad::Keys::Left);
                m_pressed_keys[static_cast<int>(Joypad::Keys::Left)] = false;
                break;
            case KEY_DOWN:
                joypad->release_key(Joypad::Keys::Down);
                m_pressed_keys[static_cast<int>(Joypad::Keys::Down)] = false;
                break;
            case KEY_RIGHT:
                joypad->release_key(Joypad::Keys::Right);
                m_pressed_keys[static_cast<int>(Joypad::Keys::Right)] = false;
                break;
            case KEY_A:
                joypad->release_key(Joypad::Keys::A);
                m_pressed_keys[static_cast<int>(Joypad::Keys::A)] = false;
                break;
            case KEY_B:
                joypad->release_key(Joypad::Keys::B);
                m_pressed_keys[static_cast<int>(Joypad::Keys::B)] = false;
                break;
            case KEY_START:
                joypad->release_key(Joypad::Keys::Start);
                m_pressed_keys[static_cast<int>(Joypad::Keys::Start)] = false;
                break;
            case KEY_SELECT:
                joypad->release_key(Joypad::Keys::Select);
                m_pressed_keys[static_cast<int>(Joypad::Keys::Select)] = false;
                break;
            }
        }
    }
}

bool Window::is_done() const {
    return m_done;
}

void Window::draw_background(const Framebuffer<graphics::gb::ColorScreen>& background,
                             std::pair<uint8_t, uint8_t> viewport_position) {
    //    const auto [img_width_pixels, img_height_pixels]
    //        = graphics::gb::tile_data_to_image(background, m_background_image, 32, 32);
    //    graphics::gb::map_gb_color_to_rgba(m_background_image);
    //    draw_rectangle_border(m_background_image, viewport_position.first,
    //    viewport_position.second,
    //                          constants::VIEWPORT_WIDTH, constants::VIEWPORT_HEIGHT, 0xd31d1d);
    m_background_image.upload_to_texture(background);

    ImGui::Begin("Background");
    auto my_tex_id = (void*)m_background_image.get_texture();
    ImGui::Image(my_tex_id, ImVec2(background.width(), background.height()));
    ImGui::End();
}

void Window::draw_sprites(const Framebuffer<graphics::gb::ColorScreen>& sprites) {
    ImGui::Begin("Sprites");
    m_sprites_image.upload_to_texture(sprites);
    auto my_tex_id = (void*)m_sprites_image.get_texture();
    ImGui::Image(my_tex_id, ImVec2(sprites.width(), sprites.height()));
    ImGui::End();
}

void Window::draw_window(const Framebuffer<graphics::gb::ColorScreen>& window) {
    ImGui::Begin("Window");
    m_window_image.upload_to_texture(window);
    auto my_tex_id = (void*)m_window_image.get_texture();
    ImGui::Image(my_tex_id, ImVec2(window.width(), window.height()));
    ImGui::End();
}

void Window::vblank_callback(const Framebuffer<graphics::gb::ColorScreen>& game) {
    m_game_image.upload_to_texture(game);
    draw_frame();
}

void Window::draw_game() {
    ImGui::Begin("Game");
    auto my_tex_id = (void*)m_game_image.get_texture();
    ImGui::Image(my_tex_id, ImVec2(m_game_image.width() * 3, m_game_image.height() * 3));
    ImGui::End();
}

void Window::draw_info(const EmulatorState& state) {
    ImGui::Begin("Info");
    auto current_ticks = SDL_GetTicks64();
    auto ms_since_last_frame = current_ticks - m_previous_ticks;
    auto fps = 0;
    if (ms_since_last_frame != 0) {
        fps = 1000 / ms_since_last_frame;
    }
    m_previous_ticks = current_ticks;
    ImGui::Text("FPS: %d", fps);
    for (int i = 0; i < 8; ++i) {
        const std::string_view state = m_pressed_keys[i] ? "Down" : "Up";
        auto name = magic_enum::enum_name(magic_enum::enum_value<Joypad::Keys>(i));
        ImGui::Text("%s", fmt::format("{}: {}", name, state).c_str());
    }
    ImGui::Text("%s", fmt::format("{} instructions elapsed", state.instructions_executed).c_str());
    ImGui::End();
}

void Window::draw_vram() {
    auto buffers = m_emulator.get_ppu()->get_tiledata();
    m_tiledata_block0.upload_to_texture(*buffers[0]);
    m_tiledata_block1.upload_to_texture(*buffers[1]);
    m_tiledata_block2.upload_to_texture(*buffers[2]);
    ImGui::Begin("Tile block 0");
    auto my_tex_id = (void*)m_tiledata_block0.get_texture();
    const auto scale = 2;
    ImGui::Image(my_tex_id,
                 ImVec2(m_tiledata_block0.width() * scale, m_tiledata_block0.height() * scale));
    ImGui::End();
    ImGui::Begin("Tile block 1");
    my_tex_id = (void*)m_tiledata_block1.get_texture();
    ImGui::Image(my_tex_id,
                 ImVec2(m_tiledata_block1.width() * scale, m_tiledata_block1.height() * scale));
    ImGui::End();
    ImGui::Begin("Tile block 2");
    my_tex_id = (void*)m_tiledata_block2.get_texture();
    ImGui::Image(my_tex_id,
                 ImVec2(m_tiledata_block2.width() * scale, m_tiledata_block2.height() * scale));
    ImGui::End();
}
