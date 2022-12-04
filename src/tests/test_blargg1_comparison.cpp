#include "catch2/catch.hpp"

#include "cpu.hpp"
#include "io.hpp"
#include "emulator.hpp"
#include "gpu.hpp"

#include "test_helpers.hpp"
#include "spdlog/spdlog.h"

#include <filesystem>

TEST_CASE("Compare blargg1 state") {
    spdlog::set_level(spdlog::level::err);
    auto expected_output = read_log_file("blargg1.txt");
    REQUIRE_FALSE(expected_output.empty());
    auto boot_rom_path = std::filesystem::absolute(("dmg01-boot.bin"));
    auto boot_rom = load_boot_rom_file(boot_rom_path);
    auto blargg_rom = load_rom_file(std::filesystem::absolute("blargg1.gb"));
    REQUIRE(boot_rom);
    Emulator emulator{blargg_rom};
    for (auto i = 0; const auto& expected_line : expected_output) {
        auto actual_output = emulator.get_debug_state();
        ++i;
        REQUIRE(actual_output == expected_line);
        REQUIRE(emulator.step());
    }
}