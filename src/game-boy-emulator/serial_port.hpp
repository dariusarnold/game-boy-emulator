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
    // Contains all the bytes written to the serial buffer during the emulators execution.
    std::string m_serial_written;

public:
    explicit SerialPort(Emulator* emulator);
    ~SerialPort();

    [[nodiscard]] std::string get_buffer() const;

    void write_byte(uint16_t address, uint8_t value);

    uint8_t read_byte(uint16_t address);
};
