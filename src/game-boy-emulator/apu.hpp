#pragma once

namespace spdlog {
class logger;
}
#include <memory>
#include <cstdint>
#include <array>

class Apu {
    std::shared_ptr<spdlog::logger> m_logger;

    // From 0xFF10-0xFF26
    std::array<uint8_t, 23> m_register_block1{};
    // From 0xFF30-0xFF3F
    std::array<uint8_t, 16> m_register_block2{};

public:
    Apu();

    uint8_t read_byte(uint16_t address);

    void write_byte(uint16_t address, uint8_t value);
};
