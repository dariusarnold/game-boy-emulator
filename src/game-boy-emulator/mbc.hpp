#pragma once

namespace spdlog {
class logger;
}
#include "cartridge_info.hpp"
#include <cstdint>
#include <vector>
#include <memory>
#include <span>

class Mbc {
    std::vector<uint8_t> m_rom;
    // A view of the memory mapped file representing the ram.
    std::span<uint8_t> m_ram;
    std::shared_ptr<spdlog::logger> m_logger;

    RomInfo m_rom_info;
    RamInfo m_ram_info;

protected:
    // Protect them to avoid polymorphic copying, which would lead to slicing.
    Mbc(const Mbc&) = default;
    Mbc(Mbc&&) = default;
    Mbc& operator=(const Mbc&) = default;
    Mbc& operator=(Mbc&&) = default;

    std::vector<uint8_t>& get_rom();
    [[nodiscard]] const std::vector<uint8_t>& get_rom() const;
    [[nodiscard]] std::span<uint8_t> get_ram();
    [[nodiscard]] std::span<const uint8_t> get_ram() const;
    [[nodiscard]] std::shared_ptr<spdlog::logger> get_logger() const;
    [[nodiscard]] const RomInfo& get_rom_info() const;
    [[nodiscard]] const RamInfo& get_ram_info() const;

public:
    [[nodiscard]] virtual uint8_t read_byte(uint16_t address) const = 0;
    virtual void write_byte(uint16_t address, uint8_t value) = 0;
    Mbc(std::vector<uint8_t> rom, std::span<uint8_t> ram);
    virtual ~Mbc();

    [[nodiscard]] static RomInfo read_rom_size_info(std::span<uint8_t> rom);
    [[nodiscard]] static RamInfo read_ram_size_info(std::span<uint8_t> rom);
};
