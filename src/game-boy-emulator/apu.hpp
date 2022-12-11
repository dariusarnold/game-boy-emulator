#pragma once

namespace spdlog {
class logger;
}
#include <memory>
#include <cstdint>

class Apu {
    std::shared_ptr<spdlog::logger> m_logger;

public:
    Apu();

    uint8_t read_byte(uint16_t address);

    void write_byte(uint16_t address, uint8_t value);
};
