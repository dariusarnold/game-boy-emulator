#include "catch2/catch.hpp"

#include "cpu.hpp"
#include "io.hpp"
#include "emulator.hpp"
#include "gpu.hpp"

#include "test_helpers.hpp"
#include "spdlog/spdlog.h"

#include <filesystem>

TEST_CASE("Compare blargg11 state") {
    spdlog::set_level(spdlog::level::err);
    auto expected_output = read_log_file("recorded-logs/blargg11.txt");
    REQUIRE_FALSE(expected_output.empty());
    auto blargg_rom = load_rom_file(std::filesystem::absolute("roms/11-op a,(hl).gb"));
    Emulator emulator{blargg_rom, {.stub_ly = true}};
    for (auto i = 0; const auto& expected_line : expected_output) {
        auto actual_output = emulator.get_debug_state();
        ++i;
        INFO("Line " << i << " " << static_cast<double>(i) / expected_output.size() * 100 << " %");
        REQUIRE(actual_output == expected_line);
        REQUIRE(emulator.step());
    }
}