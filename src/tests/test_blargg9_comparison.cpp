#include "catch2/catch.hpp"

#include "cpu.hpp"
#include "emulator.hpp"
#include "ppu.hpp"
#include "serial_port.hpp"

#include "spdlog/spdlog.h"

#include <filesystem>

TEST_CASE("Compare blargg9 state") {
    spdlog::set_level(spdlog::level::err);
    Emulator emulator{{.stub_ly = true}};
    emulator.load_game(std::filesystem::absolute("roms/09-op r,r.gb"));
    for (auto i = 0; i <= 4420382; ++i) {
        REQUIRE(emulator.step());
    }
    auto serial_content = emulator.get_serial_port()->get_buffer();
    INFO("Serial buffer " << serial_content);
    REQUIRE(serial_content.find("Passed") != std::string::npos);
}