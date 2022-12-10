#include "bitmanipulation.hpp"
#include "constants.h"
#include "emulator.hpp"
#include "io.hpp"
#include "graphics.hpp"
#include "window.hpp"
#include "gpu.hpp"

#include "fmt/format.h"
#include "fmt/ranges.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "spdlog/spdlog.h"
#include "argparse/argparse.hpp"

#include <filesystem>
#include <fstream>
#include <optional>
#include <numeric>
#include <span>
#include <cstdlib>


int main(int argc, char** argv) { // NOLINT
    argparse::ArgumentParser program("game boy emulator");
    program.add_argument("rom").help("Path to game ROM file to run.");
    program.add_argument("--boot").default_value("").help("Path to boot ROM file.");

    spdlog::set_level(spdlog::level::trace);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& e) {
        spdlog::error(e.what());
        spdlog::error(program.help().str());
        std::exit(1);
    }

    std::optional<std::array<uint8_t, 256>> boot_rom;
    if (program.is_used("boot")) {
        auto boot_rom_path = std::filesystem::absolute(program.get("boot"));
        boot_rom = load_boot_rom_file(boot_rom_path);
        if (!boot_rom) {
            spdlog::error("Failed to load boot rom from {}", boot_rom_path.string());
        }
    }
    auto rom_path = std::filesystem::absolute(program.get("rom"));
    auto game_rom = load_rom_file(rom_path);
    if (game_rom.empty()) {
        spdlog::error("Failed to load game rom from {}", rom_path.string());
        return EXIT_FAILURE;
    }

    Window window;
    std::chrono::steady_clock::time_point previous_time = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point current_time;
    double delta_seconds = 0;
    double seconds_since_last_frame = 0;

    Emulator emulator = [&]() {
        if (boot_rom.has_value()) {
            return Emulator(boot_rom.value(), game_rom);
        }
        return Emulator(game_rom);
    }();

    // Main loop
    while (!window.is_done()) {
        try {
            emulator.step();
        } catch (const std::exception& e) {
            spdlog::error("Error {}", e.what()); // NOLINT
            return EXIT_FAILURE;
        }
        current_time = std::chrono::steady_clock::now();
        delta_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - previous_time).count();
        previous_time = current_time;
        seconds_since_last_frame += delta_seconds;

        if (seconds_since_last_frame >= 1 / 59.7) {
            window.draw_frame(emulator.get_gpu()->get_vram());
            seconds_since_last_frame = 0;
        }
    }


    return EXIT_SUCCESS;
}