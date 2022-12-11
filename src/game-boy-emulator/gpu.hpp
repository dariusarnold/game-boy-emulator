#pragma once

#include "memorymap.hpp"
#include "ppu_registers.hpp"
class Emulator;
namespace spdlog {
class logger;
}
#include <array>
#include <span>
#include <memory>

class Gpu {
    std::array<uint8_t, memmap::VRamSize> m_vram{};
    PpuRegisters m_registers;
    std::shared_ptr<spdlog::logger> m_logger;
    Emulator* m_emulator;

public:
    explicit Gpu(Emulator* emulator);

    uint8_t read_byte(uint16_t address);

    void write_byte(uint16_t address, uint8_t value);

    std::span<uint8_t, 8192> get_vram();
};
