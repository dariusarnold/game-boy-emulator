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


TEST_CASE("Mooneye test suite - mbc5") {
    spdlog::set_level(spdlog::level::err);
    // The test suite recommends this value to speed up the runtime by avoiding a wait for the PPU
    Emulator emulator{{.stub_ly_value = 0xFF}};
    auto test_ended = false;
    emulator.set_debug_function([&test_ended] { test_ended = true; });

    SECTION("MBC5 rom_512kb") {
        emulator.load_game(std::filesystem::absolute("roms/mts/mbc5/rom_512kb.gb"));
        while (!test_ended) {
            emulator.step();
        }
        auto state = emulator.get_cpu()->get_debug_state();
        check_state_for_success(state);
    }

    SECTION("MBC5 rom_1Mb") {
        emulator.load_game(std::filesystem::absolute("roms/mts/mbc5/rom_1Mb.gb"));
        while (!test_ended) {
            emulator.step();
        }
        auto state = emulator.get_cpu()->get_debug_state();
        check_state_for_success(state);
    }

    SECTION("MBC5 rom_2Mb") {
        emulator.load_game(std::filesystem::absolute("roms/mts/mbc5/rom_2Mb.gb"));
        while (!test_ended) {
            emulator.step();
        }
        auto state = emulator.get_cpu()->get_debug_state();
        check_state_for_success(state);
    }

    SECTION("MBC5 rom_4Mb") {
        emulator.load_game(std::filesystem::absolute("roms/mts/mbc5/rom_4Mb.gb"));
        while (!test_ended) {
            emulator.step();
        }
        auto state = emulator.get_cpu()->get_debug_state();
        check_state_for_success(state);
    }

    SECTION("MBC5 rom_8Mb") {
        emulator.load_game(std::filesystem::absolute("roms/mts/mbc5/rom_8Mb.gb"));
        while (!test_ended) {
            emulator.step();
        }
        auto state = emulator.get_cpu()->get_debug_state();
        check_state_for_success(state);
    }

    SECTION("MBC5 rom_16Mb") {
        emulator.load_game(std::filesystem::absolute("roms/mts/mbc5/rom_16Mb.gb"));
        while (!test_ended) {
            emulator.step();
        }
        auto state = emulator.get_cpu()->get_debug_state();
        check_state_for_success(state);
    }

    SECTION("MBC5 rom_32Mb") {
        emulator.load_game(std::filesystem::absolute("roms/mts/mbc5/rom_32Mb.gb"));
        while (!test_ended) {
            emulator.step();
        }
        auto state = emulator.get_cpu()->get_debug_state();
        check_state_for_success(state);
    }

    SECTION("MBC5 rom_64Mb") {
        emulator.load_game(std::filesystem::absolute("roms/mts/mbc5/rom_64Mb.gb"));
        while (!test_ended) {
            emulator.step();
        }
        auto state = emulator.get_cpu()->get_debug_state();
        check_state_for_success(state);
    }
}
