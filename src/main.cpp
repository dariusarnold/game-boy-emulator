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

#ifdef __EMSCRIPTEN__
// Check out imgui/examples/libs/emscripten/emscripten_mainloop_stub.h for a reason
#include <emscripten.h>
#include <functional>
static std::function<void()> MainLoopForEmscriptenP;
static void MainLoopForEmscripten() {
    MainLoopForEmscriptenP();
}
#define EMSCRIPTEN_MAINLOOP_BEGIN MainLoopForEmscriptenP = [&]()
#define EMSCRIPTEN_MAINLOOP_END                                                                    \
    ;                                                                                              \
    emscripten_set_main_loop(MainLoopForEmscripten, 0, true)
#else
#define EMSCRIPTEN_MAINLOOP_BEGIN
#define EMSCRIPTEN_MAINLOOP_END
#endif


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

    Emulator emulator({});
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
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!window.is_done())
#endif
    {
        const auto& state = emulator.get_state();
        if (state.rom_file_path.has_value()) {
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
        if (emulator.get_state().new_rom_file_path.has_value()) {
            auto path = emulator.get_state().new_rom_file_path.value();
            // We need to use placement new here so the new instance is at the same address as the
            // old one. In the constructor every component gets the address of the emulator and
            // stores it to request functionality later on. If we constructed a temporary Emulator
            // and overwrote the existing object, the components would store the stack address of
            // the temporary object which would be invalid after leaving this block.
            // TODO Improve this.
            new (&emulator) Emulator({});
            emulator.load_game(path);
            emulator.set_draw_function([&]() { window.vblank_callback(); });
        }
    }
    EMSCRIPTEN_MAINLOOP_END;

    return EXIT_SUCCESS;
}