#include "io.hpp"

#include "fmt/format.h"

std::optional<std::array<uint8_t, constants::BOOT_ROM_SIZE>>
load_boot_rom_file(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        fmt::print("Boot rom file {} not found\n", path.c_str());
        return {};
    }
    std::ifstream boot_rom_file(path, std::ios::binary);
    if (!boot_rom_file.is_open()) {
        fmt::print("Could not load boot rom file\n");
        return {};
    }
    std::array<uint8_t, constants::BOOT_ROM_SIZE> boot_rom{};
    boot_rom_file.read(reinterpret_cast<char*>(boot_rom.data()), constants::BOOT_ROM_SIZE);
    if (boot_rom_file.gcount() != constants::BOOT_ROM_SIZE) {
        fmt::print("Boot rom has incorrect size {}\n", boot_rom_file.gcount());
        return {};
    }
    return boot_rom;
}

std::vector<uint8_t> load_rom_file(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        fmt::print("Rom file {} not found\n", path.c_str());
        return {};
    }
    std::ifstream rom_file(path, std::ios::binary | std::ios::ate);
    if (!rom_file.is_open()) {
        fmt::print("Could not load rom file {}\n", path.string());
        return {};
    }
    auto pos = rom_file.tellg();
    if (pos == 0) {
        return {};
    }
    std::vector<uint8_t> rom(pos);
    rom_file.seekg(0, std::ios::beg);
    rom_file.read(reinterpret_cast<char*>(rom.data()), pos);
    return rom;
}

