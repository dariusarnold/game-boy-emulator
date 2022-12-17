#include "joypad.hpp"

#include "bitmanipulation.hpp"
#include "emulator.hpp"
#include "interrupthandler.hpp"
#include <cstddef>

void Joypad::press_key(Joypad::Keys key) {
    m_key_states[static_cast<size_t>(key)] = Joypad::KeyStatus::Pressed;
    update_register();
    // Only request interrupt if the key type (Action/Movement) is enabled
    if ((bitmanip::is_bit_set(m_register, static_cast<int>(Joypad::BitValues::SelectActionButtons))
         && key >= Keys::A)
        || (bitmanip::is_bit_set(m_register,
                                 static_cast<int>(Joypad::BitValues::SelectDirectionButtons))
            && key < Keys::A)) {
        m_emulator->get_interrupt_handler()->request_interrupt(
            InterruptHandler::InterruptType::Joypad);
    }
}

void Joypad::release_key(Joypad::Keys key) {
    m_key_states[static_cast<size_t>(key)] = Joypad::KeyStatus::Released;
    update_register();
}

uint8_t Joypad::read_byte() const {
    return m_register;
}

void Joypad::write_byte(uint8_t value) {
    // The lower nibble is read only, so ignore those values to prevent errors.
    m_register = value & 0xF0;
}

void Joypad::update_register() {
    // 0 means key is pressed
    if (bitmanip::is_bit_set(m_register,
                             static_cast<int>(Joypad::BitValues::SelectActionButtons))) {
        auto key_pressed = get_key_state(Keys::A);
        bitmanip::set(m_register, static_cast<int>(Joypad::BitValues::RightOrA),
                      static_cast<bool>(key_pressed));
        key_pressed = get_key_state(Keys::B);
        bitmanip::set(m_register, static_cast<int>(Joypad::BitValues::LeftOrB),
                      static_cast<bool>(key_pressed));
        key_pressed = get_key_state(Keys::Start);
        bitmanip::set(m_register, static_cast<int>(Joypad::BitValues::DownOrStart),
                      static_cast<bool>(key_pressed));
        key_pressed = get_key_state(Keys::Select);
        bitmanip::set(m_register, static_cast<int>(Joypad::BitValues::UpOrSelect),
                      static_cast<bool>(key_pressed));
    }
    if (bitmanip::is_bit_set(m_register,
                             static_cast<int>(Joypad::BitValues::SelectDirectionButtons))) {
        auto key_pressed = get_key_state(Keys::Up);
        bitmanip::set(m_register, static_cast<int>(Joypad::BitValues::UpOrSelect),
                      static_cast<bool>(key_pressed));
        key_pressed = get_key_state(Keys::Down);
        bitmanip::set(m_register, static_cast<int>(Joypad::BitValues::DownOrStart),
                      static_cast<bool>(key_pressed));
        key_pressed = get_key_state(Keys::Left);
        bitmanip::set(m_register, static_cast<int>(Joypad::BitValues::LeftOrB),
                      static_cast<bool>(key_pressed));
        key_pressed = get_key_state(Keys::Right);
        bitmanip::set(m_register, static_cast<int>(Joypad::BitValues::RightOrA),
                      static_cast<bool>(key_pressed));
    }
}

Joypad::KeyStatus Joypad::get_key_state(Joypad::Keys key) const {
    return m_key_states[static_cast<size_t>(key)];
}

Joypad::Joypad(Emulator* emulator) : m_emulator(emulator) {
    m_key_states.fill(KeyStatus::Released);
}
