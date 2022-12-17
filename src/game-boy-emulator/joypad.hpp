#pragma once

class Emulator;
#include <cstdint>
#include <array>
#include <memory>

class Joypad {
    // 0xFF00, 1 in the bits of the lower nibble means button not pressed.
    uint8_t m_register = 0x0F;

    // The key pad is multiplexed in a 2 x 4 matrix
    enum class BitValues {
        RightOrA = 0,
        LeftOrB = 1,
        UpOrSelect = 2,
        DownOrStart = 3,
        SelectDirectionButtons = 4,
        SelectActionButtons = 5,
    };

    enum class KeyStatus {
        Pressed = 0,
        Released = 1,
    };

    // Indexed by keys, store if a key is pressed.
    std::array<KeyStatus, 8> m_key_states{};

    Emulator* m_emulator;

    void update_register();

public:
    explicit Joypad(Emulator* emulator);

    enum class Keys {
        // Movement buttons
        Up = 0,
        Down = 1,
        Left = 2,
        Right = 3,
        // Action buttons
        A = 4,
        B = 5,
        Start = 6,
        Select = 7,
    };

    void press_key(Keys key);
    void release_key(Keys key);

    [[nodiscard]] uint8_t read_byte() const;
    void write_byte(uint8_t value);

private:
    [[nodiscard]] Joypad::KeyStatus get_key_state(Joypad::Keys key) const;
};
