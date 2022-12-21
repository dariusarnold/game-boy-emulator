#include "constants.h"
#include "emulator.hpp"
#include "io.hpp"
#include "window.hpp"
#include "gpu.hpp"

#include "spdlog/spdlog.h"
#include "argparse/argparse.hpp"

#include <filesystem>
#include <optional>
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
        return EXIT_FAILURE;
    }

    std::optional<std::array<uint8_t, 256>> boot_rom;
    if (program.is_used("boot")) {
        auto boot_rom_path = std::filesystem::absolute(program.get("boot"));
        boot_rom = load_boot_rom_file(boot_rom_path);
        if (!boot_rom) {
            spdlog::error("Failed to load boot rom from {}", boot_rom_path.string());
            return EXIT_FAILURE;
        }
    }
    auto rom_path = std::filesystem::absolute(program.get("rom"));
    auto game_rom = load_rom_file(rom_path);
    if (game_rom.empty()) {
        spdlog::error("Failed to load game rom from {}", rom_path.string());
        return EXIT_FAILURE;
    }

    Emulator emulator = [&]() {
        if (boot_rom.has_value()) {
            return Emulator(boot_rom.value(), game_rom, {false});
        }
        return Emulator(game_rom, {false});
    }();

    Window window(emulator);
    emulator.set_draw_function([&](const auto& buffer) { window.vblank_callback(buffer); });

    // Main loop
    while (!window.is_done()) {
        auto sucess = emulator.step();
        if (!sucess) {
            return EXIT_FAILURE;
        }

    }

    return EXIT_SUCCESS;
}