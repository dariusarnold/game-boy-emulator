#include "mbc.hpp"

#include "spdlog/spdlog.h"
#include "exceptions.hpp"

namespace {
const int ROM_SIZE = 0x148;
const int RAM_SIZE = 0x149;
} // namespace

Mbc::Mbc(std::vector<uint8_t> rom, std::span<uint8_t> ram) :
        m_rom(std::move(rom)),
        m_ram(ram),
        m_logger(spdlog::get("")),
        m_rom_info(read_rom_size_info(m_rom)),
        m_ram_info(read_ram_size_info(m_rom)) {}

std::vector<uint8_t>& Mbc::get_rom() {
    return m_rom;
}

const std::vector<uint8_t>& Mbc::get_rom() const {
    return m_rom;
}

std::shared_ptr<spdlog::logger> Mbc::get_logger() const {
    return m_logger;
}

std::span<uint8_t> Mbc::get_ram() {
    return m_ram;
}

std::span<const uint8_t> Mbc::get_ram() const {
    return m_ram;
}

Mbc::~Mbc() = default;

const RomInfo& Mbc::get_rom_info() const {
    return m_rom_info;
}

const RamInfo& Mbc::get_ram_info() const {
    return m_ram_info;
}

RomInfo Mbc::read_rom_size_info(std::span<uint8_t> rom) {
    auto val = rom[ROM_SIZE];
    if (val > 0x8) {
        throw LogicError("Invalid value for ROM size");
    }
    return {static_cast<size_t>((32 * 1024) * (1 << val)), static_cast<size_t>(1 << (val + 1))};
}

RamInfo Mbc::read_ram_size_info(std::span<uint8_t> rom) {
    auto val = rom[RAM_SIZE];
    switch (val) {
    case 0:
        return {0, 0};
    case 2:
        return {8 * 1024, 1};
    case 3:
        return {32 * 1024, 4};
    case 4:
        return {128 * 1024, 16};
    case 5:
        return {64 * 1024, 8};
    default:
        throw LogicError("Invalid value for RAM size");
    }
}
