#pragma once

#include <cstddef>

class ClockTimer {
    size_t m_clock_count = 0;
    size_t m_clock_amount = 0;
    size_t m_trigger_count = 0;

public:
    explicit ClockTimer(size_t clock_count);
    bool tick();

    [[nodiscard]] size_t get_trigger_count() const;
};
