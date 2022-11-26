#pragma once

class IMemoryRange;
#include "memory_range.hpp"

class InterruptHandler {
    bool interrupt_master_enable_flag;

public:
    InterruptHandler();

    void set_enable_flag(bool enable);
};
