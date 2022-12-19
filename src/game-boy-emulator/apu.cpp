#include "apu.hpp"
#include "spdlog/spdlog.h"


Apu::Apu(): m_logger(spdlog::get("")) {}

uint8_t Apu::read_byte(uint16_t address) {
    m_logger->debug("APU: Unhandled read at {:04X}", address);
    return 0xFF;
}

void Apu::write_byte(uint16_t address, uint8_t) {
    m_logger->debug("APU: Unhandled write at {:04X}", address);
}
