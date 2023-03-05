#include <numeric>
#include "window.hpp"
#include "graphics.hpp"
#include "emulator.hpp"
#include "ppu.hpp"
#include "joypad.hpp"

#include "spdlog/spdlog.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "SDL.h"
#include "nfd.hpp"

Window::Window(Emulator& emulator) :
        m_emulator(emulator), m_logger(spdlog::get("")), m_fps_history(5 * 60, 5 * 60, 0) {
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a
    // minority of Windows systems, depending on whether SDL_INIT_GAMECONTROLLER is enabled or
    // disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0) {
        spdlog::error("Error: {}", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }

    auto window_flags
        = static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    m_sdl_window = SDL_CreateWindow("game boy emulator", SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, 1280, 720, window_flags);
    m_sdl_renderer = SDL_CreateRenderer(m_sdl_window, -1,
                                        SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    // Initialize nativefiledialog-extended
    if (NFD::Init() != NFD_OKAY) {
        spdlog::error("Failed to initialize native filedialog");
        std::exit(EXIT_FAILURE);
    }

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

    NFD::Quit();

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
constexpr SDL_KeyCode KEY_HOLD_FAST_FORWARD = SDLK_SPACE;
constexpr SDL_KeyCode KEY_TOGGLE_FAST_FORWARD = SDLK_f;
constexpr SDL_KeyCode KEY_FAST_FORWARD_INCREASE = SDLK_q;
constexpr SDL_KeyCode KEY_FAST_FORWARD_DECREASE = SDLK_e;

void toggle(bool& b) {
    b = !b;
}

} // namespace

void Window::draw_frame() {
    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    draw_menubar();

    auto& options = m_emulator.get_options();
    if (options.draw_debug_background) {
        draw_background();
    }
    if (options.draw_debug_window) {
        draw_window();
    }
    if (options.draw_debug_sprites) {
        draw_sprites();
    }
    if (options.draw_info_window) {
        draw_info(m_emulator.get_state());
    }
    if (options.draw_debug_tiles) {
        draw_vram();
    }

    draw_game();

    // Rendering
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGui::Render();
    SDL_SetRenderDrawColor(m_sdl_renderer, static_cast<Uint8>(clear_color.x * 255),
                           static_cast<Uint8>(clear_color.y * 255),
                           static_cast<Uint8>(clear_color.z * 255),
                           static_cast<Uint8>(clear_color.w * 255));
    SDL_RenderClear(m_sdl_renderer);
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(m_sdl_renderer);
}

void Window::handle_user_keyboard_input(const SDL_Event& event,
                                        const std::shared_ptr<Joypad>& joypad) {
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
            case KEY_HOLD_FAST_FORWARD:
            case KEY_TOGGLE_FAST_FORWARD:
                toggle(m_emulator.get_options().fast_forward);
                break;
            case KEY_FAST_FORWARD_INCREASE:
                m_emulator.get_options().game_speed++;
                m_emulator.get_options().fast_forward = true;
                break;
            case KEY_FAST_FORWARD_DECREASE:
                if (m_emulator.get_options().game_speed > 1) {
                    m_emulator.get_options().game_speed--;
                }
                m_emulator.get_options().fast_forward = m_emulator.get_options().game_speed > 1;
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
            case KEY_HOLD_FAST_FORWARD:
                toggle(m_emulator.get_options().fast_forward);
                break;
            }
        }
    }
}

bool Window::is_done() const {
    return m_done;
}

void Window::draw_background() {
    const auto& background = m_emulator.get_ppu()->get_background();
    m_background_image.upload_to_texture(background);
    auto& options = m_emulator.get_options();
    ImGui::Begin("Background", &options.draw_debug_background, ImGuiWindowFlags_NoResize);
    auto* my_tex_id = static_cast<void*>(m_background_image.get_texture());
    ImGui::Image(my_tex_id, ImVec2(static_cast<float>(background.width()),
                                   static_cast<float>(background.height())));
    ImGui::End();
}

void Window::draw_sprites() {
    const auto& sprites = m_emulator.get_ppu()->get_sprites();
    m_sprites_image.upload_to_texture(sprites);
    auto& options = m_emulator.get_options();
    ImGui::Begin("Sprites", &options.draw_debug_sprites, ImGuiWindowFlags_NoResize);
    auto* my_tex_id = static_cast<void*>(m_sprites_image.get_texture());
    ImGui::Image(my_tex_id,
                 ImVec2(static_cast<float>(sprites.width()), static_cast<float>(sprites.height())));
    ImGui::End();
}

void Window::draw_window() {
    const auto& window = m_emulator.get_ppu()->get_window();
    auto& options = m_emulator.get_options();
    ImGui::Begin("Window", &options.draw_debug_window, ImGuiWindowFlags_NoResize);
    m_window_image.upload_to_texture(window);
    auto* my_tex_id = static_cast<void*>(m_window_image.get_texture());
    ImGui::Image(my_tex_id,
                 ImVec2(static_cast<float>(window.width()), static_cast<float>(window.height())));
    ImGui::End();
}

void Window::vblank_callback() {
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

    const auto& state = m_emulator.get_state();
    const auto& options = m_emulator.get_options();
    const auto& game = m_emulator.get_ppu()->get_game();
    if (options.fast_forward) {
        if (state.frame_count % options.game_speed == 0) {
            m_game_image.upload_to_texture(game);
            draw_frame();
        }
    } else {
        m_game_image.upload_to_texture(game);
        draw_frame();
    }
}

void Window::draw_game() {
    ImGui::Begin(m_emulator.get_state().game_title.c_str(), nullptr, ImGuiWindowFlags_NoResize);
    auto* my_tex_id = static_cast<void*>(m_game_image.get_texture());
    ImGui::Image(my_tex_id, ImVec2(static_cast<float>(m_game_image.width() * 3),
                                   static_cast<float>(m_game_image.height() * 3)));
    ImGui::End();
}

void Window::draw_info(const EmulatorState& state) {
    auto& options = m_emulator.get_options();
    ImGui::Begin("Info", &options.draw_info_window, ImGuiWindowFlags_NoResize);
    auto current_ticks = SDL_GetTicks64();
    auto ms_since_last_frame = current_ticks - m_previous_ticks;
    auto fps = 0.f;
    if (ms_since_last_frame != 0) {
        fps = 1000.f / static_cast<float>(ms_since_last_frame);
    }
    m_fps_history.push_back(fps);
    // ImGui requires continuous storage of the data to be plotted.
    m_fps_history.linearize();
    auto avg_fps = std::accumulate(m_fps_history.end() - 5, m_fps_history.end(), 0.) / 5;
    ImGui::PlotLines("FPS", &m_fps_history[0], static_cast<int>(m_fps_history.size()), 0,
                     fmt::format("{} FPS", avg_fps).c_str(), 0, 120, ImVec2{500, 100});
    m_previous_ticks = current_ticks;
    for (int i = 0; i < 8; ++i) {
        const std::string_view key_state = m_pressed_keys[i] ? "Down" : "Up";
        auto name = magic_enum::enum_name(magic_enum::enum_value<Joypad::Keys>(i));
        ImGui::Text("%s", fmt::format("{}: {}", name, key_state).c_str()); // NOLINT
    }
    // NOLINTNEXTLINE
    ImGui::Text("%s", fmt::format("{} instructions elapsed", state.instructions_executed).c_str());
    ImGui::Text("Speed %d", options.game_speed); // NOLINT
    ImGui::End();
}

void Window::draw_vram() {
    auto buffers = m_emulator.get_ppu()->get_tiledata();
    m_tiledata_block0.upload_to_texture(*buffers[0]);
    m_tiledata_block1.upload_to_texture(*buffers[1]);
    m_tiledata_block2.upload_to_texture(*buffers[2]);
    auto& options = m_emulator.get_options();
    ImGui::Begin("Tile block 0,1,2", &options.draw_debug_tiles, ImGuiWindowFlags_NoResize);
    auto* my_tex_id = static_cast<void*>(m_tiledata_block0.get_texture());
    const auto scale = 2;
    ImGui::Image(my_tex_id, ImVec2(static_cast<float>(m_tiledata_block0.width() * scale),
                                   static_cast<float>(m_tiledata_block0.height() * scale)));
    ImGui::SameLine();
    my_tex_id = static_cast<void*>(m_tiledata_block1.get_texture());
    ImGui::Image(my_tex_id, ImVec2(static_cast<float>(m_tiledata_block1.width() * scale),
                                   static_cast<float>(m_tiledata_block1.height() * scale)));
    my_tex_id = static_cast<void*>(m_tiledata_block2.get_texture());
    ImGui::SameLine();
    ImGui::Image(my_tex_id, ImVec2(static_cast<float>(m_tiledata_block2.width() * scale),
                                   static_cast<float>(m_tiledata_block2.height() * scale)));
    ImGui::End();
}

void Window::draw_menubar_file() {
    if (ImGui::MenuItem("Load game")) {
        NFD::UniquePath out_path;
        nfdresult_t const result = NFD::OpenDialog(out_path);
        if (result == NFD_OKAY) {
            std::filesystem::path const p{out_path.get()};
            m_emulator.get_state().new_rom_file_path = p;
        }
    }
    if (ImGui::MenuItem("Quit")) {
        m_done = true;
    }
    ImGui::EndMenu();
}

namespace {

void draw_menubar_settings_ppu(EmulatorOptions& options) {
    if (ImGui::MenuItem("Toggle info window")) {
        toggle(options.draw_info_window);
    }
    if (ImGui::MenuItem("Toggle debug background")) {
        toggle(options.draw_debug_background);
    }
    if (ImGui::MenuItem("Toggle debug window")) {
        toggle(options.draw_debug_window);
    }
    if (ImGui::MenuItem("Toggle debug sprites")) {
        toggle(options.draw_debug_sprites);
    }
    if (ImGui::MenuItem("Toggle debug tile viewer")) {
        toggle(options.draw_debug_tiles);
    }
    ImGui::EndMenu();
}

void draw_menubar_settings_speed(EmulatorOptions& options) {
    if (ImGui::RadioButton("Speed 1", &options.game_speed, 1)) {
        options.fast_forward = false;
        options.game_speed = 1;
        ImGui::CloseCurrentPopup();
    }
    if (ImGui::RadioButton("Speed 2", &options.game_speed, 2)) {
        options.fast_forward = true;
        ImGui::CloseCurrentPopup();
    }
    if (ImGui::RadioButton("Speed 3", &options.game_speed, 3)) {
        options.fast_forward = true;
        ImGui::CloseCurrentPopup();
    }
    if (ImGui::RadioButton("Speed 4", &options.game_speed, 4)) {
        options.fast_forward = true;
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndMenu();
}

void draw_menubar_settings_sound(EmulatorOptions& options, float volume) {
    ImGui::SliderFloat("Volume", &volume, 0, 1);
    options.volume = volume;
    auto label
        = fmt::format("Toggle channel 1 (Now {})", options.apu_channel1_enabled ? "ON" : "OFF");
    if (ImGui::MenuItem(label.c_str())) {
        toggle(options.apu_channel1_enabled);
    }
    label = fmt::format("Toggle channel 2 (Now {})", options.apu_channel2_enabled ? "ON" : "OFF");
    if (ImGui::MenuItem(label.c_str())) {
        toggle(options.apu_channel2_enabled);
    }
    label = fmt::format("Toggle channel 3 (Now {})", options.apu_channel2_enabled ? "ON" : "OFF");
    if (ImGui::MenuItem(label.c_str())) {
        toggle(options.apu_channel3_enabled);
    }
    label = fmt::format("Toggle channel 4 (Now {})", options.apu_channel2_enabled ? "ON" : "OFF");
    if (ImGui::MenuItem(label.c_str())) {
        toggle(options.apu_channel4_enabled);
    }

    ImGui::EndMenu();
}

} // namespace

void Window::draw_menubar_settings() {
    auto& options = m_emulator.get_options();
    if (ImGui::BeginMenu("PPU debug")) {
        draw_menubar_settings_ppu(options);
    }
    if (ImGui::BeginMenu("Emulation speed")) {
        draw_menubar_settings_speed(options);
    }
    if (ImGui::BeginMenu("Sound")) {
        float volume = m_emulator.get_options().volume;
        draw_menubar_settings_sound(options, volume);
    }

    ImGui::EndMenu();
}

void Window::draw_menubar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            draw_menubar_file();
        }
        if (ImGui::BeginMenu("Settings")) {
            draw_menubar_settings();
        }
        ImGui::EndMainMenuBar();
    }
}
