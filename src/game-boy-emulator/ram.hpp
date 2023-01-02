#pragma once

#include "memorymap.hpp"

#include <array>
#include <cstdint>
#include <memory>

namespace spdlog {
class logger;
}
class Emulator;


class Ram {
    std::array<uint8_t, memmap::InternalRamSize> internalRam{};
    std::array<uint8_t, memmap::HighRamSize> highRam{};
    Emulator* m_emulator;
    std::shared_ptr<spdlog::logger> m_logger;

public:
    explicit Ram(Emulator* emulator);

    /**
     * Read memory value from address.
     */
    [[nodiscard]] uint8_t read_byte(uint16_t address) const;

    /**
     * Write value to ram at address.
     */
    void write_byte(uint16_t address, uint8_t value);
};
