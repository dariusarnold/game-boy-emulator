#pragma once

class InterruptHandler {
    bool interrupt_master_enable_flag = false;

public:
    void set_enable_flag(bool enable);
};
