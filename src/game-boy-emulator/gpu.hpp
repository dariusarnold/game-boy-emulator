#pragma once

#include "memory_range.hpp"


class Gpu {
    MemoryRange vram;
public:
    Gpu();

    uint8_t read_byte(uint16_t address);

    void write_byte(uint16_t address, uint8_t value);
};
