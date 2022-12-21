#include "joypad.hpp"

#include "bitmanipulation.hpp"
#include "emulator.hpp"
#include "interrupthandler.hpp"
#include <cstddef>

void Joypad::press_key(Joypad::Keys key) {
    set_key_state(key, KeyStatus::Pressed);
    // Only request interrupt if the key type (Action/Movement) is enabled
    if ((!bitmanip::is_bit_set(m_register, static_cast<int>(Joypad::BitValues::SelectActionButtons))
         && key >= Keys::A)
        || (!bitmanip::is_bit_set(m_register,
                                  static_cast<int>(Joypad::BitValues::SelectDirectionButtons))
            && key < Keys::A)) {
        if (m_emulator != nullptr) {
            m_emulator->get_interrupt_handler()->request_interrupt(
                InterruptHandler::InterruptType::Joypad);
        }
    }
}

void Joypad::release_key(Joypad::Keys key) {
    set_key_state(key, KeyStatus::Released);
}

uint8_t Joypad::read_byte() {
    // Update register content on the fly
    auto select_action = !bitmanip::is_bit_set(
        m_register, static_cast<int>(Joypad::BitValues::SelectActionButtons));
    auto select_direction = !bitmanip::is_bit_set(
        m_register, static_cast<int>(Joypad::BitValues::SelectDirectionButtons));
    for (unsigned i = 0; i < 4; ++i) {
        auto pressed_direction = m_key_states[i];
        auto pressed_action = m_key_states[i + 4];

        if (select_action && select_direction) {
            bitmanip::set_bit_value(m_register, i,
                                    static_cast<uint8_t>(pressed_action)
                                        & static_cast<uint8_t>(pressed_direction));
        } else if (select_action) {
            bitmanip::set_bit_value(m_register, i, static_cast<uint8_t>(pressed_action));
        } else if (select_direction) {
            bitmanip::set_bit_value(m_register, i, static_cast<uint8_t>(pressed_direction));
        }
    }
    return m_register;
}

void Joypad::write_byte(uint8_t value) {
    // The lower nibble is read only and the upper two bits are unused, so ignore those values to
    // prevent errors.
    auto select_action_bit
        = bitmanip::bit_value(value, static_cast<int>(Joypad::BitValues::SelectActionButtons));
    auto select_direction_bit
        = bitmanip::bit_value(value, static_cast<int>(Joypad::BitValues::SelectDirectionButtons));
    bitmanip::set_bit_value(m_register, static_cast<int>(Joypad::BitValues::SelectActionButtons),
                            select_action_bit);
    bitmanip::set_bit_value(m_register, static_cast<int>(Joypad::BitValues::SelectDirectionButtons),
                            select_direction_bit);
}

Joypad::KeyStatus Joypad::get_key_state(Joypad::Keys key) const {
    return static_cast<KeyStatus>(m_key_states[static_cast<size_t>(key)]);
}

Joypad::Joypad(Emulator* emulator) : m_emulator(emulator) {
    m_key_states.fill(KeyStatus::Released);
}

void Joypad::set_key_state(Joypad::Keys key, Joypad::KeyStatus status) {
    m_key_states[static_cast<size_t>(key)] = status;
}
