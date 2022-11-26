#include "gpu.hpp"
#include "exceptions.hpp"
#include "memorymap.hpp"

Gpu::Gpu() :
        vram(memmap::VRamBegin, memmap::VRamEnd),
        bg_palette_data(0xFF47, 0xFF47),
        scroll_y(0xFF42, 0xFF42),
        lcd_control(0xFF40, 0xFF40),
        lcd_y_coordinate(0xFF44, 0xFF44){
    lcd_y_coordinate.write_byte(0xFF44, 0x90);
}


std::vector<IMemoryRange*> Gpu::get_mappable_memory() {
    return {&vram, &bg_palette_data, &scroll_y, &lcd_control, &lcd_y_coordinate};
}

uint8_t Gpu::read_byte(uint16_t address) {
    if (memmap::isIn(address, memmap::VRam)) {
        return vram.read_byte(address);
    } else {
        throw LogicError(fmt::format("GPU can't read from {:04X}", address));
    }
}

void Gpu::write_byte(uint16_t address, uint8_t value) {
    if (memmap::isIn(address, memmap::VRam)) {
        vram.write_byte(address, value);
    } else {
        throw LogicError(fmt::format("GPU can't write to {:04X}", address));
    }
}
