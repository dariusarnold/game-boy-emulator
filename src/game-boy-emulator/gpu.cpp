#include "gpu.hpp"
#include "exceptions.hpp"
#include "memorymap.hpp"
#include "emulator.hpp"
#include "fmt/format.h"
#include "spdlog/spdlog.h"
#include <span>

Gpu::Gpu(Emulator* emulator): m_logger(spdlog::get("")), m_emulator(emulator) {}

namespace {
const uint16_t ADDRESS_LY_REGISTER = 0xFF44;
const uint16_t ADDRESS_SCY_REGISTER = 0xFF42;
}

uint8_t Gpu::read_byte(uint16_t address) {
    if (memmap::is_in(address, memmap::VRam)) {
        return vram[address - memmap::VRamBegin];
    }
    if (address == ADDRESS_LY_REGISTER && m_emulator->get_options().stub_ly) {
        return 0x90;
    }
    if (address == ADDRESS_SCY_REGISTER) {
        return m_scy_register;
    }
    throw LogicError(fmt::format("GPU can't read from {:04X}", address));
}

void Gpu::write_byte(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::VRam)) {
        vram[address - memmap::VRamBegin] = value;
    } else if (address == ADDRESS_SCY_REGISTER) {
        m_scy_register = value;
    } else {
        m_logger->error("GPU: unhandled write to {:04X}", address);
    }
}

std::span<uint8_t, 8192> Gpu::get_vram() {
    return {vram};
}
