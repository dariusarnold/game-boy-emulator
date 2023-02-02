#pragma once


struct EmulatorOptions {
    // Used for unit test cpu state comparison. Fix LY (0xFF44) constantly at the given value if
    // this value is non negative.
    int stub_ly_value = -1;
    bool draw_info_window = true;
    bool draw_debug_background = true;
    bool draw_debug_window = true;
    bool draw_debug_sprites = true;
    bool draw_debug_tiles = true;
    bool fast_forward = false;
    int game_speed = 1;
    float volume = 0.1;
};
