#include "catch2/catch.hpp"

#include "io.hpp"
#include "emulator.hpp"
#include "gpu.hpp"
#include "serial_port.hpp"

#include "test_helpers.hpp"
#include "spdlog/spdlog.h"

#include <filesystem>

TEST_CASE("Compare instr_timing state") {
    spdlog::set_level(spdlog::level::err);
    auto expected_output = read_log_file("recorded-logs/instr-timing-log.txt");
    REQUIRE_FALSE(expected_output.empty());
    auto game_rom = load_rom_file(std::filesystem::absolute("roms/instr_timing.gb"));
    Emulator emulator{game_rom, {true}};
    for (auto i = 0; const auto& expected_line : expected_output) {
        auto actual_output = emulator.get_debug_state();
        ++i;
        INFO("Line " << i << " " << static_cast<double>(i) / expected_output.size() * 100 << " %");
        //REQUIRE(actual_output == expected_line);
        REQUIRE(emulator.step());
    }
    auto serial_content = emulator.get_serial_port()->get_buffer();
    INFO("Serial buffer " << serial_content);
    REQUIRE(serial_content.find("Passed") != std::string::npos);
}