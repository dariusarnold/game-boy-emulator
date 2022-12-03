#pragma once

class Emulator;
namespace spdlog {
class logger;
}
#include <cstdint>
#include <memory>

class InterruptHandler {
public:
    explicit InterruptHandler(Emulator* emulator);
    enum class InterruptEnabledStatus {
        Disabled,
        Enabled,
    };

    void callback_instruction_elapsed();

    [[nodiscard]] bool get_interrupt_enable_status() const;
    void set_global_interrupt_enabled(bool enabled);

    void write_interrupt_enable(uint8_t val);

private:
    // Enabling interrupts is delayed by one instruction.
    int8_t m_global_enabled_instruction_countdown = -1;
    bool m_global_interrupt_enabled_status = false;
    // 0xFFFF
    uint8_t m_interrupt_enable_register = 0;
    // 0xFF0F
    uint8_t m_interrupt_request_flags = 0;
    Emulator* m_emulator;
    std::shared_ptr<spdlog::logger> logger;
};
