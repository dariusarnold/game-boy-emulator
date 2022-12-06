#include "timer.hpp"

#include "emulator.hpp"
#include "exceptions.hpp"
#include "bitmanipulation.hpp"

#include "spdlog/spdlog.h"

#include <limits>


Timer::Timer(Emulator* emulator) : m_emulator(emulator), m_logger(spdlog::get("")) {}

namespace {
constexpr int CLOCK_RATE_M = 4'194'304;
// The cycle count is in M cycles, meaning it is a quarter of the actual clock rate/the T cycle
// count. This means after one M cycle, 4 T cycles have elapsed. Since the divider is given by T
// frequency, the number of cycles has to be divided by 4 to know after how many M cycles a timer
// should increment.
constexpr int CLOCK_RATE_T = CLOCK_RATE_M / 4;
constexpr std::array<uint32_t, 4> N_CYCLES_TIMER_COUNTER{
    CLOCK_RATE_T / 4096, CLOCK_RATE_T / 262144, CLOCK_RATE_T / 65536, CLOCK_RATE_T / 16384};
} // namespace

void Timer::cycle_elapsed_callback(size_t cycle_num) {
    if (cycle_num % N_CYCLES_TIMER_COUNTER[3] == 0) {
        m_divider_register += 1;
    }
    if (bitmanip::is_bit_set(m_timer_control, 2)) {
        auto every_n_cycles = N_CYCLES_TIMER_COUNTER[m_timer_control & 0b11];
        if (cycle_num % every_n_cycles == 0) {
            if (m_timer_counter == std::numeric_limits<decltype(m_timer_counter)>::max()) {
                // Overflow would occur on next increment, reset and trigger interrupt
                m_timer_counter = m_timer_modulo;
                m_emulator->get_interrupt_handler()->request_interrupt(
                    InterruptHandler::InterruptType::Timer);
            } else {
                m_timer_counter += 1;
            }
        }
    }
}

namespace {
constexpr uint16_t ADDRESS_DIVIDER_REGISTER = 0xFF04;
constexpr uint16_t ADDRESS_TIMER_MODULO = 0xFF06;
constexpr uint16_t ADDRESS_TIMER_CONTROL = 0xFF07;
} // namespace

void Timer::write_byte(uint16_t address, uint8_t value) {
    if (address == ADDRESS_DIVIDER_REGISTER) {
        // Any value resets the divider to 0.
        m_logger->debug("Reset timer DIV");
        m_divider_register = 0;
    } else if (address == ADDRESS_TIMER_CONTROL) {
        m_logger->debug("Set timer control {:03b}", m_timer_control);
        m_timer_control = value;
    } else if (address == ADDRESS_TIMER_MODULO) {
        m_logger->debug("Set timer modulo {:02X}", m_timer_modulo);
        m_timer_modulo = value;
    } else if (address == 0xFF05) {
        m_logger->debug("Set TIMA to {:02X}", value);
        m_timer_counter = value;
    } else {
        throw LogicError(fmt::format("Invalid write of {:02X} in timer to {:04X}", value, address));
    }
}
