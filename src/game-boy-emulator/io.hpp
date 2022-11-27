#pragma once

#include "constants.h"

#include <filesystem>
#include <optional>
#include <array>
#include <fstream>

std::optional<std::array<uint8_t, constants::BOOT_ROM_SIZE>>
load_boot_rom_file(const std::filesystem::path& path);

std::vector<uint8_t> load_rom_file(const std::filesystem::path& path);
