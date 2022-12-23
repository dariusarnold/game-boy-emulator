#include "mbc.hpp"

#include "spdlog/spdlog.h"

Mbc::Mbc(std::vector<uint8_t> rom, std::span<uint8_t> ram) :
        m_rom(std::move(rom)), m_ram(ram), m_logger(spdlog::get("")) {}

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
