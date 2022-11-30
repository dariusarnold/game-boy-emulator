#include <vector>
#include <spdlog/spdlog.h>
#include "io.hpp"

#include "fmt/format.h"

std::optional<std::array<uint8_t, constants::BOOT_ROM_SIZE>>
load_boot_rom_file(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        spdlog::error("Boot rom file {} not found", path.c_str());
        return std::nullopt;
    }
    std::ifstream boot_rom_file(path, std::ios::binary);
    if (!boot_rom_file.is_open()) {
        spdlog::error("Could not load boot rom file");
        return std::nullopt;
    }
    std::array<uint8_t, constants::BOOT_ROM_SIZE> boot_rom{};
    boot_rom_file.read(std::bit_cast<char*>(boot_rom.data()), constants::BOOT_ROM_SIZE);
    if (boot_rom_file.gcount() != constants::BOOT_ROM_SIZE) {
        spdlog::error("Boot rom has incorrect size {}", boot_rom_file.gcount());
        return std::nullopt;
    }
    return boot_rom;
}

std::vector<uint8_t> load_rom_file(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        spdlog::error("Rom file {} not found", path.c_str());
        return {};
    }
    std::ifstream rom_file(path, std::ios::binary | std::ios::ate);
    if (!rom_file.is_open()) {
        spdlog::error("Could not load rom file {}", path.string());
        return {};
    }
    auto pos = rom_file.tellg();
    if (pos <= 0) {
        return {};
    }
    std::vector<uint8_t> rom(static_cast<size_t>(pos));
    rom_file.seekg(0, std::ios::beg);
    rom_file.read(std::bit_cast<char*>(rom.data()), pos);
    return rom;
}
