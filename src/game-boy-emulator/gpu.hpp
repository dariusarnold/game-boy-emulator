#pragma once

#include "memory_range.hpp"


class Gpu {
    MemoryRange vram;
    MemoryRange bg_palette_data;
    MemoryRange scroll_y;
    MemoryRange lcd_control;
    MemoryRange lcd_y_coordinate;
public:
    Gpu();

    uint8_t read_byte(uint16_t address);

    void write_byte(uint16_t address, uint8_t value);

    std::vector<IMemoryRange*> get_mappable_memory();
};
