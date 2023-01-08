#include "catch2/catch.hpp"

#include "cpu.hpp"
#include "emulator.hpp"
#include "serial_port.hpp"

#include "spdlog/spdlog.h"

#include <filesystem>

namespace {
void check_state_for_success(const CpuDebugState& state) {
    // Pass fail reporting of the mooneye test roms:
    // On success write 3,5,8,13,21,24 to the serial port
    REQUIRE(state.b == 3);
    REQUIRE(state.c == 5);
    REQUIRE(state.d == 8);
    REQUIRE(state.e == 13);
    REQUIRE(state.h == 21);
    REQUIRE(state.l == 34);
}
} // namespace


TEST_CASE("Mooneye test suite - acceptance/oam_dma") {
    spdlog::set_level(spdlog::level::err);
    // The test suite recommends this value to speed up the runtime by avoiding a wait for the PPU
    Emulator emulator{{.stub_ly_value = 0xFF}};
    auto test_ended = false;
    emulator.set_debug_function([&test_ended] { test_ended = true; });

    SECTION("oam_dma basic") {
        emulator.load_game(std::filesystem::absolute("roms/mts/acceptance/oam_dma/basic.gb"));
        while (!test_ended) {
            emulator.step();
        }
        auto state = emulator.get_cpu()->get_debug_state();
        check_state_for_success(state);
    }

    SECTION("oam_dma reg_read") {
        emulator.load_game(std::filesystem::absolute("roms/mts/acceptance/oam_dma/reg_read.gb"));
        while (!test_ended) {
            emulator.step();
        }
        auto state = emulator.get_cpu()->get_debug_state();
        check_state_for_success(state);
    }

    SECTION("sources-GS basic") {
        emulator.load_game(std::filesystem::absolute("roms/mts/acceptance/oam_dma/sources-GS.gb"));
        while (!test_ended) {
            emulator.step();
        }
        auto state = emulator.get_cpu()->get_debug_state();
        check_state_for_success(state);
    }
}
