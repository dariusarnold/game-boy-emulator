#pragma once

#include <array>
#include <cstdint>
#include <vector>

/**
 * Main class for emulation, instantiates all parts, connects and manages them.
 */
class Emulator {
public:
    Emulator(const std::array<uint8_t, 256>& boot_rom, const std::vector<uint8_t>& game_rom);
    void run();

private:
    std::array<uint8_t, 256> m_boot_rom;
    std::vector<uint8_t> m_game_rom;
};