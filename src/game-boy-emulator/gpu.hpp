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



    std::vector<IMemoryRange*> get_mappable_memory();
};
