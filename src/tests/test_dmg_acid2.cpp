#include "catch2/catch.hpp"

#include "image.hpp"
#include "emulator.hpp"
#include "ppu.hpp"
#include "constants.h"

#include "spdlog/spdlog.h"
#include "SDL_surface.h"


TEST_CASE("dmg_acid2") {
    spdlog::set_level(spdlog::level::err);
    Emulator emulator{{}};
    emulator.load_game("roms/dmg-acid2.gb");
    auto test_ended = false;
    Framebuffer<graphics::gb::ColorScreen, constants::SCREEN_RES_WIDTH,
                constants::SCREEN_RES_HEIGHT>
        actual_framebuffer;
    emulator.set_debug_function([&test_ended] { test_ended = true; });

    // Run until test signals end. Ignore all frames until then.
    while (!test_ended) {
        REQUIRE(emulator.step());
    }
    // Get the next framebuffer before it is reset.
    test_ended = false;
    emulator.set_draw_function([&] {
        actual_framebuffer = emulator.get_ppu()->get_game();
        test_ended = true;
    });
    while (!test_ended) {
        REQUIRE(emulator.step());
    }

    // Convert the framebuffer to an SDL_Surface to save it for convenient comparisons if the tests
    // fail.
    auto* actual_image = actual_framebuffer.to_surface();
    SDL_SaveBMP(actual_image, "dmg-acid2-actual.bmp");

    // Load the known-good screenshot from a file
    auto* expected_image = SDL_LoadBMP("recorded-logs/dmg-acid2.bmp");
    expected_image = SDL_ConvertSurface(expected_image, actual_image->format, 0);
    REQUIRE(expected_image != nullptr);
    Framebuffer<graphics::gb::ColorScreen, constants::SCREEN_RES_WIDTH,
                constants::SCREEN_RES_HEIGHT>
        result;
    auto res = SDL_LockSurface(expected_image);
    REQUIRE(res == 0);
    REQUIRE(expected_image->h * expected_image->pitch
            == result.size() * sizeof(graphics::gb::ColorScreen));
    result.take_from(expected_image->pixels);
    SDL_UnlockSurface(expected_image);
    // Compare the actual screenshot to the known-good one
    for (size_t x = 0; x < result.width(); ++x) {
        for (size_t y = 0; y < result.height(); ++y) {
            INFO(fmt::format("Coordinate {}/{} differs", x, y));
            CHECK(actual_framebuffer.get_pixel(x, y) == result.get_pixel(x, y));
        }
    }
}