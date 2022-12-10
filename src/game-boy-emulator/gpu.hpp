#pragma once

#include "memorymap.hpp"

#include <array>
#include <span>

class Gpu {
    std::array<uint8_t, memmap::VRamSize> vram{};

public:
    Gpu();

    uint8_t read_byte(uint16_t address);

    void write_byte(uint16_t address, uint8_t value);

    std::span<uint8_t, 8192> get_vram();
};
