#include "catch2/catch.hpp"

#include "cpu.hpp"
#include "io.hpp"
#include "emulator.hpp"
#include "ppu.hpp"
#include "serial_port.hpp"

#include "test_helpers.hpp"
#include "spdlog/spdlog.h"

#include <filesystem>

TEST_CASE("Compare blargg1 state") {
    spdlog::set_level(spdlog::level::err);
    Emulator emulator{{.stub_ly = true}};
    emulator.load_game(std::filesystem::absolute("roms/01-special.gb"));
    for (auto i = 0; i <= 1258895; ++i) {
        REQUIRE(emulator.step());
    }
    auto serial_content = emulator.get_serial_port()->get_buffer();
    INFO("Serial buffer " << serial_content);
    REQUIRE(serial_content.find("Passed") != std::string::npos);

}