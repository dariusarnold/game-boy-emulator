#include "catch2/catch.hpp"

#include "emulator.hpp"
#include "serial_port.hpp"

#include "test_helpers.hpp"
#include "spdlog/spdlog.h"

#include <filesystem>

TEST_CASE("blargg mem_timing") {
    spdlog::set_level(spdlog::level::err);
    Emulator emulator{{true}};
    emulator.load_game(std::filesystem::absolute("roms/mem_timing.gb"));
    for (auto i = 0; i <= 813000; ++i) {
        REQUIRE(emulator.step());
    }
    auto serial_content = emulator.get_serial_port()->get_buffer();
    INFO("Serial buffer " << serial_content);
    REQUIRE(serial_content.find("Passed") != std::string::npos);
}