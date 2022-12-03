#pragma once

class Emulator;
namespace spdlog {
class logger;
}
#include <cstdint>
#include <memory>

class InterruptHandler {
public:
    enum class InterruptType : uint8_t {
        VBlank,
        LCD,
        Timer,
        SerialLink,
        Joypad,
    };

    explicit InterruptHandler(Emulator* emulator);

    void callback_instruction_elapsed();
    void handle_interrupts();

    [[nodiscard]] bool get_global_interrupt_enable_status() const;
    void set_global_interrupt_enabled(bool enabled);

    void write_interrupt_enable(uint8_t val);
    // Overwrites all current values in the interrupt flag register with the new value.
    void write_interrupt_flag(uint8_t val);
    // Only overwrites the specific interrupt types bit in the interrupt flag register, leaving all
    // other values intact.
    void request_interrupt(InterruptType interrupt_type);

private:
    // Enabling interrupts is delayed by one instruction.
    int8_t m_global_enabled_instruction_countdown = -1;
    bool m_global_interrupt_enabled_status = false;
    // 0xFFFF
    uint8_t m_interrupt_enable_register = 0;
    // 0xFF0F
    uint8_t m_interrupt_request_flags = 0;
    Emulator* m_emulator;
    std::shared_ptr<spdlog::logger> m_logger;
};
