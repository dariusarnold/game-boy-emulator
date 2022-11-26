#include "gpu.hpp"
#include "exceptions.hpp"
#include "memorymap.hpp"

#include "fmt/format.h"

Gpu::Gpu() :
        vram{} {}

uint8_t Gpu::read_byte(uint16_t address) {
    if (memmap::is_in(address, memmap::VRam)) {
        return vram[address - memmap::VRamBegin];
    } else {
        throw LogicError(fmt::format("GPU can't read from {:04X}", address));
    }
}

void Gpu::write_byte(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::VRam)) {
        vram[address - memmap::VRamBegin] = value;
    } else {
        throw LogicError(fmt::format("GPU can't write to {:04X}", address));
    }
}
