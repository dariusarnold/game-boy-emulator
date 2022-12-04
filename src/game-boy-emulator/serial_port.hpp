#pragma once

class Emulator;
namespace spdlog {
class logger;
}
#include <memory>

class SerialPort {
    Emulator* m_emulator;
    std::shared_ptr<spdlog::logger> m_logger;

    uint8_t m_serial_buffer = 0;
    uint8_t m_serial_control = 0;
    std::string out;

public:
    explicit SerialPort(Emulator* emulator);
    ~SerialPort();

    void write_byte(uint16_t address, uint8_t value);

    uint8_t read_byte(uint16_t address);
};
