#pragma once

#include "memorymap.hpp"
class Emulator;
namespace spdlog {
class logger;
}
#include <array>
#include <span>
#include <memory>

class Gpu {
    std::array<uint8_t, memmap::VRamSize> vram{};
    std::shared_ptr<spdlog::logger> m_logger;
    Emulator* m_emulator;

    uint8_t m_scy_register = 0;

public:
    explicit Gpu(Emulator* emulator);

    uint8_t read_byte(uint16_t address);

    void write_byte(uint16_t address, uint8_t value);

    std::span<uint8_t, 8192> get_vram();
};
