#include "interrupthandler.hpp"

#include "emulator.hpp"
#include "bitmanipulation.hpp"
#include "cpu.hpp"

#include "magic_enum.hpp"
#include "spdlog/spdlog.h"

InterruptHandler::InterruptHandler(Emulator* emulator) :
        m_emulator(emulator), m_logger(spdlog::get("")) {}

bool InterruptHandler::get_global_interrupt_enable_status() const {
    return m_global_interrupt_enabled_status;
}

void InterruptHandler::set_global_interrupt_enabled(bool enabled) {
    if (m_global_interrupt_enabled_status) {
        return;
    }
    if (enabled != m_global_interrupt_enabled_status) {
        m_global_enabled_instruction_countdown = 1;
        m_logger->debug("Schedule global interrupt enable change to {}", enabled);
    }
}

void InterruptHandler::callback_instruction_elapsed() {
    if (m_global_enabled_instruction_countdown > 0) {
        m_global_enabled_instruction_countdown--;
    } else if (m_global_enabled_instruction_countdown == 0) {
        m_global_interrupt_enabled_status = !m_global_interrupt_enabled_status;
        m_logger->debug("Global interrupt enabled {}", m_global_interrupt_enabled_status);
        // Set negative to avoid retriggering on the next instruction
        m_global_enabled_instruction_countdown = -1;
    }
}

void InterruptHandler::write_interrupt_enable(uint8_t val) {
    m_logger->debug("Write interrupt enable VBlank {}, LCD STAT {}, Timer {}, Serial {}, Joypad {}",
                    bitmanip::is_bit_set(val, 0), bitmanip::is_bit_set(val, 1),
                    bitmanip::is_bit_set(val, 2), bitmanip::is_bit_set(val, 3),
                    bitmanip::is_bit_set(val, 4));
    m_interrupt_enable_register = val;
}

void InterruptHandler::write_interrupt_flag(uint8_t val) {
    m_logger->debug("Write interrupt flag VBlank {}, LCD STAT {}, Timer {}, Serial {}, Joypad {}",
                    bitmanip::is_bit_set(val, 0), bitmanip::is_bit_set(val, 1),
                    bitmanip::is_bit_set(val, 2), bitmanip::is_bit_set(val, 3),
                    bitmanip::is_bit_set(val, 4));
    m_interrupt_request_flags = val;
}

namespace {
// Sorted by priority (high to low): VBlank, LCD STAT, Timer, Serial, Joypad
const std::array<uint16_t, 5> ISR_ADDRESS{0x0040, 0x0048, 0x0050, 0x0058, 0x0060};
} // namespace

void InterruptHandler::handle_interrupts() {
    for (unsigned i = 0; i < 5; ++i) {
        if (bitmanip::is_bit_set(m_interrupt_enable_register, i)
            && bitmanip::is_bit_set(m_interrupt_request_flags, i)) {
            // Always unhalt even if interrupts are disabled globally
            m_emulator->unhalt();
            if (m_global_interrupt_enabled_status) {
                m_global_interrupt_enabled_status = false;
                bitmanip::unset(m_interrupt_request_flags, i);
                auto address = ISR_ADDRESS[i];
                m_logger->debug("Serving interrupt {}",
                                magic_enum::enum_name(static_cast<InterruptType>(i)));
                m_emulator->get_cpu()->call_isr(address);
            }
            break;
        }
    }
}

void InterruptHandler::request_interrupt(InterruptHandler::InterruptType interrupt_type) {
    m_logger->debug("Request interrupt {}", magic_enum::enum_name(interrupt_type));
    auto new_flag = m_interrupt_request_flags | static_cast<uint8_t>(interrupt_type);
    write_interrupt_flag(new_flag);
}

uint8_t InterruptHandler::read_interrupt_enable() const {
    return m_interrupt_enable_register;
}

uint8_t InterruptHandler::read_interrupt_flag() const {
    return m_interrupt_request_flags;
}
