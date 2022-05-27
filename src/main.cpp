#include "constants.h"
#include "emulator.hpp"
#include "io.hpp"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include <filesystem>
#include <fstream>
#include <optional>


template <typename Container>
void print_container_hex(const Container& c) {
    fmt::print("[{:02X}]\n", fmt::join(c, ", "));
}


int main() {
    auto boot_rom_path = std::filesystem::absolute(("../../dmg01-boot.bin"));
    auto boot_rom = load_boot_rom_file(boot_rom_path);
    if (!boot_rom) {
        return -1;
    }

    print_container_hex(boot_rom.value());
    Emulator emulator(boot_rom.value());
    emulator.run();
    return 0;
}
