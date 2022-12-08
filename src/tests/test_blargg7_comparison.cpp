#include "catch2/catch.hpp"

#include "cpu.hpp"
#include "io.hpp"
#include "emulator.hpp"
#include "gpu.hpp"
#include "serial_port.hpp"

#include "test_helpers.hpp"
#include "spdlog/spdlog.h"

#include <filesystem>

TEST_CASE("Compare blargg7 state") {
    spdlog::set_level(spdlog::level::err);
    auto expected_output = read_log_file("recorded-logs/blargg7.txt");
    REQUIRE_FALSE(expected_output.empty());
    auto blargg_rom = load_rom_file(std::filesystem::absolute("roms/07-jr,jp,call,ret,rst.gb"));
    Emulator emulator{blargg_rom};
    for (auto i = 0; const auto& expected_line : expected_output) {
        auto actual_output = emulator.get_debug_state();
        ++i;
        INFO("Line " << i << " " << static_cast<double>(i) / expected_output.size() * 100 << " %");
        REQUIRE(emulator.step());
    }
    auto serial_content = emulator.get_serial_port()->get_buffer();
    INFO("Serial buffer " << serial_content);
    REQUIRE(serial_content.find("Passed") != std::string::npos);
}