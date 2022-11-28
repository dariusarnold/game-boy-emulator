#pragma once

#include <array>
#include <cstdint>
#include "constants.h"

class Emulator;


class BootRom {
    Emulator* m_emulator;
    std::array<uint8_t, constants::BOOT_ROM_SIZE> m_rom;

public:
    BootRom(Emulator* emulator, std::array<uint8_t, constants::BOOT_ROM_SIZE> rom);

    [[nodiscard]] uint8_t read_byte(uint16_t address) const;
};
