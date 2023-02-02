#include "emulator.hpp"
#include "window.hpp"
#include "ppu.hpp"
#include "apu.hpp"
#include "audio.hpp"

#include "spdlog/spdlog.h"
#include "argparse/argparse.hpp"

#include <filesystem>
#include <optional>
#include <cstdlib>


int main(int argc, char** argv) { // NOLINT
    argparse::ArgumentParser program("game boy emulator");
    program.add_argument("rom").help("Path to game ROM file to run.");
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
    emulator.set_draw_function([&]() { window.vblank_callback(); });

    Audio audio(emulator);
    emulator.set_audio_function([&](SampleFrame sample){audio.callback(sample);});

    // Main loop
    while (!window.is_done()) {
        auto sucess = emulator.step();
        if (!sucess) {
            return EXIT_FAILURE;
        }
        if (emulator.get_state().new_rom_file_path.has_value()) {
            auto path = emulator.get_state().new_rom_file_path.value();
            // We need to use placement new here so the new instance is at the same address as the
            // old one. In the constructor every component gets the address of the emulator and
            // stores it to request functionality later on. If we constructed a temporary Emulator
            // and overwrote the existing object, the components would store the stack address of
            // the temporary object which would be invalid after leaving this block.
            // TODO Improve this.
            new (&emulator) Emulator({false});
            emulator.load_game(path);
            emulator.set_draw_function([&]() { window.vblank_callback(); });
        }
    }

    return EXIT_SUCCESS;
}