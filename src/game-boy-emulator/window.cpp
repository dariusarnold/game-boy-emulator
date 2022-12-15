#include "window.hpp"
#include "graphics.hpp"
#include "emulator.hpp"
#include "gpu.hpp"
#include "constants.h"
#include "joypad.hpp"

#include "spdlog/spdlog.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "SDL.h"

Window::Window() :
        m_tile_data_image(24 * 8, 16 * 8),
        m_background_image(constants::BACKGROUND_SIZE_PIXELS, constants::BACKGROUND_SIZE_PIXELS),
        m_window_image(constants::BACKGROUND_SIZE_PIXELS, constants::BACKGROUND_SIZE_PIXELS) {
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

    m_tile_data_image.init_texture(m_sdl_renderer);
    m_background_image.init_texture(m_sdl_renderer);
    m_window_image.init_texture(m_sdl_renderer);
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

void Window::draw_frame(const Emulator& emulator) {
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
        handle_user_keyboard_input(event, emulator.get_joypad());
    }

    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    auto& io = ImGui::GetIO();

    draw_tile_data_viewer(emulator.get_gpu()->get_vram_tile_data());

    draw_background(emulator.get_gpu()->get_background(),
                    emulator.get_gpu()->get_viewport_position());
    draw_window(emulator.get_gpu()->get_window());

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
        if (event.type == SDL_KEYDOWN && event.key.repeat != 0) {
            switch (event.key.keysym.sym) {
            case KEY_UP:
                joypad->press_key(Joypad::Keys::Up);
                break;
            case KEY_LEFT:
                joypad->press_key(Joypad::Keys::Left);
                break;
            case KEY_DOWN:
                joypad->press_key(Joypad::Keys::Down);
                break;
            case KEY_RIGHT:
                joypad->press_key(Joypad::Keys::Right);
                break;
            case KEY_A:
                joypad->press_key(Joypad::Keys::A);
                break;
            case KEY_B:
                joypad->press_key(Joypad::Keys::B);
                break;
            case KEY_START:
                joypad->press_key(Joypad::Keys::Start);
                break;
            case KEY_SELECT:
                joypad->press_key(Joypad::Keys::Select);
                break;
            }
        }
        if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
            case KEY_UP:
                joypad->release_key(Joypad::Keys::Up);
                break;
            case KEY_LEFT:
                joypad->release_key(Joypad::Keys::Left);
                break;
            case KEY_DOWN:
                joypad->release_key(Joypad::Keys::Down);
                break;
            case KEY_RIGHT:
                joypad->release_key(Joypad::Keys::Right);
                break;
            case KEY_A:
                joypad->release_key(Joypad::Keys::A);
                break;
            case KEY_B:
                joypad->release_key(Joypad::Keys::B);
                break;
            case KEY_START:
                joypad->release_key(Joypad::Keys::Start);
                break;
            case KEY_SELECT:
                joypad->release_key(Joypad::Keys::Select);
                break;
            }
        }
    }
}

bool Window::is_done() const {
    return m_done;
}

void Window::draw_tile_data_viewer(std::span<uint8_t, memmap::TileDataSize> vram) {
    auto& io = ImGui::GetIO();
    // Our state
    float image_scale = 4;
    const auto [img_width_pixels, img_height_pixels]
        = graphics::gb::tile_data_to_image(vram, m_tile_data_image, 24, 16);
    graphics::gb::map_gb_color_to_rgba(m_tile_data_image);
    m_tile_data_image.upload_to_texture();

    ImGui::Begin("Tile data");
    auto my_tex_id = (void*)m_tile_data_image.get_texture();
    float my_tex_w = img_width_pixels * image_scale;
    float my_tex_h = img_height_pixels * image_scale;
    ImGui::Text("%.0fx%.0f", my_tex_w, my_tex_h);
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
    ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
    ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
    ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
    auto min = ImGui::GetItemRectMin();
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Image x %f y %f", io.MousePos.x - min.x, io.MousePos.y - min.y);
        float tooltip_zoom = 3;
        float region_sz = 32.0f;
        float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
        float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
        if (region_x < 0.0f) {
            region_x = 0.0f;
        } else if (region_x > my_tex_w - region_sz) {
            region_x = my_tex_w - region_sz;
        }
        if (region_y < 0.0f) {
            region_y = 0.0f;
        } else if (region_y > my_tex_h - region_sz) {
            region_y = my_tex_h - region_sz;
        }
        ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y) / my_tex_h);
        ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
        ImGui::Image(my_tex_id,
                     ImVec2(region_sz * (image_scale + tooltip_zoom),
                            region_sz * (image_scale + tooltip_zoom)),
                     uv0, uv1, tint_col, border_col);
        ImGui::EndTooltip();
    }
    ImGui::End();
}

void Window::draw_background(std::vector<uint8_t> background,
                             std::pair<uint8_t, uint8_t> viewport_position) {
    // 32x32 tiles, each 16 bytes in size
    assert(background.size() == 32 * 32 * 16 && "Background size check");
    const auto [img_width_pixels, img_height_pixels]
        = graphics::gb::tile_data_to_image(background, m_background_image, 32, 32);
    graphics::gb::map_gb_color_to_rgba(m_background_image);
    draw_rectangle_border(m_background_image, viewport_position.first, viewport_position.second,
                          constants::VIEWPORT_WIDTH, constants::VIEWPORT_HEIGHT, 0xd31d1d);
    m_background_image.upload_to_texture();

    ImGui::Begin("Background");
    auto my_tex_id = (void*)m_background_image.get_texture();
    ImGui::Image(my_tex_id, ImVec2(img_width_pixels, img_height_pixels));
    ImGui::End();
}

void Window::draw_window(std::vector<uint8_t> window) {
    // 32x32 tiles, each 16 bytes in size
    assert(window.size() == 32 * 32 * 16 && "Window size check");
    const auto [img_width_pixels, img_height_pixels]
        = graphics::gb::tile_data_to_image(window, m_window_image, 32, 32);
    graphics::gb::map_gb_color_to_rgba(m_window_image);
    m_window_image.upload_to_texture();

    ImGui::Begin("Window");
    auto my_tex_id = (void*)m_window_image.get_texture();
    ImGui::Image(my_tex_id, ImVec2(img_width_pixels, img_height_pixels));
    ImGui::End();
}
