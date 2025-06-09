#include "cartridge.hpp"
#include <catch2/catch.hpp>

#include "io.hpp"


TEST_CASE("Test reading empty game title from cartridge", "[cartridge]") {
    EmulatorIo io;
    auto rom_path = std::filesystem::absolute("roms/stub-game.gb");
    auto rom = io.load_rom_file(rom_path);
    auto title = cartridge::get_title(rom.value());
    REQUIRE(title.size() == 0);
}

TEST_CASE("Test reading valid game title from cartridge", "[cartridge]") {
    EmulatorIo io;
    auto rom_path = std::filesystem::absolute("roms/stub-game-normal-title.gb");
    auto rom = io.load_rom_file(rom_path);
    auto title = cartridge::get_title(rom.value());
    REQUIRE(title == "My Game Title2!");
}

TEST_CASE("Test title being truncated to 16 characters", "[cartridge]") {
    EmulatorIo io;
    auto rom_path = std::filesystem::absolute("roms/stub-game-long-title.gb");
    auto rom = io.load_rom_file(rom_path);
    auto title = cartridge::get_title(rom.value());
    REQUIRE(title == "My Game Title2!i");
}
