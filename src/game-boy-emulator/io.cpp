#include "io.hpp"
#include <vector>
#include "spdlog/spdlog.h"


std::optional<std::array<uint8_t, constants::BOOT_ROM_SIZE>>
EmulatorIo::load_boot_rom_file(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        m_logger->error("Boot rom file {} not found", path.c_str());
        return std::nullopt;
    }
    std::ifstream boot_rom_file(path, std::ios::binary);
    if (!boot_rom_file.is_open()) {
        m_logger->error("Could not load boot rom file");
        return std::nullopt;
    }
    std::array<uint8_t, constants::BOOT_ROM_SIZE> boot_rom{};
    boot_rom_file.read(std::bit_cast<char*>(boot_rom.data()), constants::BOOT_ROM_SIZE);
    if (boot_rom_file.gcount() != constants::BOOT_ROM_SIZE) {
        m_logger->error("Boot rom has incorrect size {}", boot_rom_file.gcount());
        return std::nullopt;
    }
    return boot_rom;
}

std::optional<std::vector<uint8_t>> EmulatorIo::load_rom_file(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        m_logger->error("Rom file {} not found", path.c_str());
        return std::nullopt;
    }
    std::ifstream rom_file(path, std::ios::binary | std::ios::ate);
    if (!rom_file.is_open()) {
        m_logger->error("Could not load rom file {}", path.string());
        return std::nullopt;
    }
    auto pos = rom_file.tellg();
    if (pos <= 0) {
        m_logger->error("Error loading rom file {}", path.string());
        return std::nullopt;
    }
    std::vector<uint8_t> rom(static_cast<size_t>(pos));
    rom_file.seekg(0, std::ios::beg);
    rom_file.read(std::bit_cast<char*>(rom.data()), pos);
    return rom;
}

EmulatorIo::EmulatorIo() : m_logger(spdlog::get("")) {}

void EmulatorIo::create_file(const std::filesystem::path& path, size_t filesize_bytes) {
    std::ofstream file(path.string());
    std::fill_n(std::ostream_iterator<char>(file), filesize_bytes, 0);
}
