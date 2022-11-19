#pragma once

class IMemoryRange;
#include "memory_range.hpp"

class InterruptHandler {
    bool interrupt_master_enable_flag;
    MemoryRange interrupt_flags;
    MemoryRange interrupt_enable;

public:
    InterruptHandler();

    void set_enable_flag(bool enable);

    std::vector<IMemoryRange*> get_mappable_memory();
};
