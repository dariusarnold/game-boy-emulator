#include "catch2/catch.hpp"

#include "cpu.hpp"
#include "io.hpp"
#include "emulator.hpp"
#include "ppu.hpp"

#include "test_helpers.hpp"

#include "fmt/format.h"
#include "spdlog/spdlog.h"

#include <filesystem>


TEST_CASE("Compare boot sequence") {
    spdlog::set_level(spdlog::level::err);
    auto expected_output = read_log_file("recorded-logs/BootromLog.txt");
    REQUIRE_FALSE(expected_output.empty());
    auto boot_rom_path = std::filesystem::absolute("roms/dmg_boot.gb");
    std::vector<uint8_t> cartridge(1024, 0);
    // clang-format off
    const std::vector<uint8_t> cartridge_header{0x00, 0xC3, 0x1, 0x50 /* entry point */,
                                                0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00,
                                                0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD,
                                                0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB,
                                                0xB9, 0x33, 0x3E /* logo */,
                                                0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 /* game title (should be 16 bytes, but the boot rom log only uses 15 */,
                                                0x80 /* cgb flag*/,
                                                0x0,
                                                0x0 /* licensee code */,
                                                0x0 /* sgb flag*/,
                                                0x1 /* cartridge type */,
                                                0x0 /* rom size*/,
                                                0x0 /* ram size */,
                                                0x0 /* destination code */,
                                                0x0 /* old licensee code */,
                                                0x0 /* version number */,
                                                0x66 /* header checksum */,
                                                0x0, 0x0 /* global checksum */};
    // clang-format on
    for (auto i = 0; i < cartridge_header.size(); ++i) {
        cartridge[i + 0x100] = cartridge_header[i];
    }
    Emulator emulator{{.stub_ly = true}};
    emulator.load_boot(boot_rom_path);
    for (auto i = 0; const auto& expected_line : expected_output) {
        auto actual_output = emulator.get_debug_state();
        ++i;
        REQUIRE(actual_output == expected_line);
        REQUIRE(emulator.step());
    }
}