#include "constants.h"
#include "cpu.hpp"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include <filesystem>
#include <fstream>
#include <optional>


template <typename Container>
void print_container_hex(const Container& c) {
    fmt::print("[{:02X}]\n", fmt::join(c, ", "));
}


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


int main() {
    auto boot_rom_path = std::filesystem::absolute(("../../dmg01-boot.bin"));
    auto boot_rom = load_boot_rom_file(boot_rom_path);
    if (!boot_rom) {
        return -1;
    }
    print_container_hex(boot_rom.value());
    Cpu cpu;
    cpu.set_boot_rom(boot_rom.value());
    cpu.run();
    return 0;
}
