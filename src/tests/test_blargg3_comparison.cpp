#include "catch2/catch.hpp"

#include "emulator.hpp"
#include "serial_port.hpp"

#include "spdlog/spdlog.h"

#include <filesystem>

TEST_CASE("Compare blargg3 state") {
    spdlog::set_level(spdlog::level::err);
    Emulator emulator{{.stub_ly_value = 0x90}};
    emulator.load_game(std::filesystem::absolute("roms/03-op sp,hl.gb"));
    for (auto i = 0; i <= 1068422; ++i) {
        REQUIRE(emulator.step());
    }
    auto serial_content = emulator.get_serial_port()->get_buffer();
    INFO("Serial buffer " << serial_content);
    REQUIRE(serial_content.find("Passed") != std::string::npos);
}