#pragma once

class Emulator;
namespace spdlog {
class logger;
}
#include <cstdint>
#include <memory>


/**
 * Dispatch memory access to the correct component.
 */
class AddressBus {
    Emulator* m_emulator;
    std::shared_ptr<spdlog::logger> m_logger;

public:
    explicit AddressBus(Emulator* emulator);

    /**
     * Read memory value from address.
     */
    [[nodiscard]] uint8_t read_byte(uint16_t address) const;

    /**
     * Write value to ram at address.
     */
    void write_byte(uint16_t address, uint8_t value);
};
