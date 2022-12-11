#include "gpu.hpp"
#include "exceptions.hpp"
#include "memorymap.hpp"
#include "emulator.hpp"
#include "fmt/format.h"
#include "spdlog/spdlog.h"

#include <span>

Gpu::Gpu(Emulator* emulator) :
        m_registers(emulator->get_options().stub_ly),
        m_logger(spdlog::get("")),
        m_emulator(emulator) {}

uint8_t Gpu::read_byte(uint16_t address) {
    if (memmap::is_in(address, memmap::VRam)) {
        return m_vram[address - memmap::VRamBegin];
    }
    if (memmap::is_in(address, memmap::PpuIoRegisters)) {
        return m_registers.get_register_value(address);
    }

    throw LogicError(fmt::format("GPU can't read from {:04X}", address));
}

void Gpu::write_byte(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::VRam)) {
        m_vram[address - memmap::VRamBegin] = value;
    } else if (memmap::is_in(address, memmap::PpuIoRegisters)) {
        if (address == 0xFF46) {
            throw NotImplementedError("OAM DMA access");
        }
        m_registers.set_register_value(address, value);
    } else {
        m_logger->error("GPU: unhandled write to {:04X}", address);
    }
}

std::span<uint8_t, 8192> Gpu::get_vram() {
    return {m_vram};
}
