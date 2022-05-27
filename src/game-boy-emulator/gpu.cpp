#include "gpu.hpp"


Gpu::Gpu() :
        vram(0x8000, 0x9FFF),
        bg_palette_data(0xFF47, 0xFF47),
        scroll_y(0xFF42, 0xFF42),
        lcd_control(0xFF40, 0xFF40),
        lcd_y_coordinate(0xFF44, 0xFF44){
    lcd_y_coordinate.write_byte(0xFF44, 0x90);
}


std::vector<IMemoryRange*> Gpu::get_mappable_memory() {
    return {&vram, &bg_palette_data, &scroll_y, &lcd_control, &lcd_y_coordinate};
}
