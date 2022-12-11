#pragma once


struct EmulatorOptions {
    // Used for unit test cpu state comparison. Fix LX 0xFF44 constantly at 144 (VBlank).
    bool stub_ly = false;
};
