#include "emulator.hpp"
#include "window.hpp"
#include "ppu.hpp"
#include "apu.hpp"
#include "audio.hpp"

#include "spdlog/spdlog.h"
#include "argparse/argparse.hpp"

#include <exception>
#include <filesystem>
#include <optional>
#include <cstdlib>
#include <spdlog/common.h>


int main(int argc, char** argv) { // NOLINT
    argparse::ArgumentParser program("game boy emulator");
    program.add_argument("--game").help("Path to game ROM file to run.");
    program.add_argument("--boot").default_value("").help("Path to boot ROM file.");

    spdlog::set_level(spdlog::level::info);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& e) {
        spdlog::error(e.what());
        spdlog::error(program.help().str());
        return EXIT_FAILURE;
    }

    std::optional<std::filesystem::path> boot_rom_path;
    std::optional<std::filesystem::path> game_rom_path;
    if (program.is_used("boot")) {
        boot_rom_path = std::filesystem::absolute(program.get("boot"));
    }
    if (program.is_used("game")) {
        game_rom_path = std::filesystem::absolute(program.get("game"));
    }

    Emulator emulator{{}};
    if (boot_rom_path.has_value() and game_rom_path.has_value()) {
        emulator.load_boot_game(boot_rom_path.value(), game_rom_path.value());
    } else if (game_rom_path.has_value()) {
        emulator.load_game(game_rom_path.value());
    } else if (boot_rom_path.has_value()) {
        // This means only a boot rom without a game was given
        spdlog::error("Running boot rom without a game rom not supported.");
        std::exit(1);
    }

    Window window(emulator);
    emulator.set_draw_function([&]() { window.vblank_callback(); });

    Audio audio(emulator);
    if (audio.is_working()) {
        emulator.set_audio_function([&](SampleFrame sample) { audio.callback(sample); });
    }

    // Main loop
    while (!window.is_done()) {
        if (emulator.get_state().rom_file_path.has_value()) {
            auto sucess = emulator.step();
            if (!sucess) {
                return EXIT_FAILURE;
            }
        } else {
            // Normally the vblank interrupt would trigger this callback. But since we are in a
            // state where no game is loaded and the emulator is not running, we have to call it
            // manually for the UI to render
            window.vblank_callback();
        }
        const auto& state = emulator.get_state();
        if (state.new_rom_file_path.has_value()) {
            audio.clear_queued_samples();
            auto path = state.new_rom_file_path.value();
            emulator.get_state().new_rom_file_path = std::nullopt;
            emulator.reset_state();
            emulator.load_game(path);
        }
    }

    return EXIT_SUCCESS;
}
