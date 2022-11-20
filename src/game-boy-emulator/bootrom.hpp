#pragma once

#include <array>
#include <cstdint>

class Emulator;


class BootRom {
    std::array<uint8_t, 256> m_rom;
    Emulator* m_emulator;
public:
    BootRom(Emulator* emulator, std::array<uint8_t, 256> rom);

    uint8_t read_byte(uint16_t address) const;

};