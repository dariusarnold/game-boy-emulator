#pragma once

class Emulator;
namespace spdlog {
class logger;
}
#include <memory>
#include <cstdint>

class Timer {
    Emulator* m_emulator;
    // DIV 0xFF04
    uint8_t m_divider_register = 0;
    // TIMA 0xFF05
    uint8_t m_timer_counter = 0;
    // TMA 0xFF06
    uint8_t m_timer_modulo = 0;
    // TAC 0xFF07
    uint8_t m_timer_control = 0;
    std::shared_ptr<spdlog::logger> m_logger;

public:
    explicit Timer(Emulator* emulator);

    // Called after every m cycle
    void cycle_elapsed_callback(size_t cycle_num);

    void write_byte(uint16_t address, uint8_t value);
};
