#pragma once

#include <array>

struct EmulatorOptions {
    // Used for unit test cpu state comparison. Fix LY (0xFF44) constantly at the given value if
    // this value is non negative.
    int stub_ly_value = -1;
    // Info window displays the emulators stats such as pressed buttons or the frame rate.
    bool draw_info_window = true;
    // The following allow to disable a specific "layer" of the graphic renderer.
    bool draw_debug_background = true;
    bool draw_debug_window = true;
    bool draw_debug_sprites = true;
    // Draw the complete tile map of the PPU
    bool draw_debug_tiles = true;
    // Controls fast forward
    bool fast_forward = false;
    // Fast forward multiplier
    int game_speed = 1;
    // Global sound volume
    float volume = 0.1f;
    // Allows toggling the output of a channel of the APU on/off for sound debugging purposes.
    bool apu_channel1_enabled = true;
    bool apu_channel2_enabled = true;
    bool apu_channel3_enabled = true;
    bool apu_channel4_enabled = true;
};
