#pragma once

#include "memory_range.hpp"


class Apu {
    MemoryRange sound;

public:
    Apu();

    IMemoryRange* get_mappable_memory();

};
