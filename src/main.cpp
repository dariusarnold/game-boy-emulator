#include "emulator.hpp"
#include "window.hpp"
#include "ppu.hpp"

#include "spdlog/spdlog.h"
#include "argparse/argparse.hpp"

#include <filesystem>
#include <optional>
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

    std::optional<std::filesystem::path> boot_rom_path;
    if (program.is_used("boot")) {
        boot_rom_path = std::filesystem::absolute(program.get("boot"));
    }
    auto rom_path = std::filesystem::absolute(program.get("rom"));

    Emulator emulator({false});
    if (boot_rom_path.has_value()) {
        emulator.load_boot_game(boot_rom_path.value(), rom_path);
    } else {
        emulator.load_game(rom_path);
    }

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