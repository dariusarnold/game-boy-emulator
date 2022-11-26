#pragma once

#include <array>
#include <cstdint>

class Emulator;


class BootRom {
    Emulator* m_emulator;
    std::array<uint8_t, 256> m_rom;
public:
    BootRom(Emulator* emulator, std::array<uint8_t, 256> rom);

    [[nodiscard]] uint8_t read_byte(uint16_t address) const;

};