#include "catch2/catch.hpp"

#include "serial_port.hpp"
#include "emulator.hpp"

#include "test_helpers.hpp"
#include "spdlog/spdlog.h"

#include <filesystem>

TEST_CASE("Compare blargg8 state") {
    spdlog::set_level(spdlog::level::err);
    Emulator emulator{{.stub_ly_value = 0x90}};
    emulator.load_game(std::filesystem::absolute("roms/08-misc instrs.gb"));
    for (auto i = 0; i <= 223892; ++i) {
        REQUIRE(emulator.step());
    }
    auto serial_content = emulator.get_serial_port()->get_buffer();
    INFO("Serial buffer " << serial_content);
    REQUIRE(serial_content.find("Passed") != std::string::npos);
}