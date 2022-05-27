#include "apu.hpp"


Apu::Apu(): sound(0xFF10, 0xFF26) {}

IMemoryRange* Apu::get_mappable_memory() {
    return &sound;
}
