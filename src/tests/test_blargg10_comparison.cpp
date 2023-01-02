#include "catch2/catch.hpp"

#include "cpu.hpp"
#include "serial_port.hpp"
#include "emulator.hpp"
#include "ppu.hpp"

#include "spdlog/spdlog.h"

#include <filesystem>

TEST_CASE("Compare blargg10 state") {
    spdlog::set_level(spdlog::level::err);
    Emulator emulator{{.stub_ly = true}};
    emulator.load_game(std::filesystem::absolute("roms/10-bit ops.gb"));
    for (auto i = 0; i <= 6714723; ++i) {
        REQUIRE(emulator.step());
    }
    auto serial_content = emulator.get_serial_port()->get_buffer();
    INFO("Serial buffer " << serial_content);
    REQUIRE(serial_content.find("Passed") != std::string::npos);
}