#pragma once

#include "constants.h"

#include <filesystem>
#include <optional>
#include <array>
#include <fstream>
#include <vector>
namespace spdlog {
class logger;
}


class EmulatorIo {
    std::shared_ptr<spdlog::logger> m_logger;

public:
    EmulatorIo();

    std::optional<std::array<uint8_t, constants::BOOT_ROM_SIZE>>
    load_boot_rom_file(const std::filesystem::path& path);

    std::optional<std::vector<uint8_t>> load_rom_file(const std::filesystem::path& path);
};
