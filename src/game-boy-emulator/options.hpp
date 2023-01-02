#pragma once


struct EmulatorOptions {
    // Used for unit test cpu state comparison. Fix LX 0xFF44 constantly at 144 (VBlank).
    bool stub_ly = false;
    bool draw_info_window = true;
    bool draw_debug_background = true;
    bool draw_debug_window = true;
    bool draw_debug_sprites = true;
    bool draw_debug_tiles = true;
};
