#include "clocktimer.hpp"

ClockTimer::ClockTimer(size_t clock_count) :
        m_clock_count(clock_count), m_clock_amount(clock_count) {}

bool ClockTimer::tick() {
    m_clock_count--;
    if (m_clock_count == 0) {
        m_clock_count = m_clock_amount;
        m_trigger_count++;
        return true;
    }
    return false;
}

size_t ClockTimer::get_trigger_count() const {
    return m_trigger_count;
}
