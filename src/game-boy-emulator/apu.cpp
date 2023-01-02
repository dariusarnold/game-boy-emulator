#include "apu.hpp"
#include "spdlog/spdlog.h"
#include "memorymap.hpp"


Apu::Apu() : m_logger(spdlog::get("")) {}

uint8_t Apu::read_byte(uint16_t address) {
    if (memmap::is_in(address, memmap::Apu)) {
        return m_register_block1[address - memmap::ApuBegin];
    }
    if (memmap::is_in(address, memmap::WavePattern)) {
        return m_register_block2[address - memmap::WavePatternBegin];
    }
    m_logger->debug("APU: Unhandled read at {:04X}", address);
    return 0xFF;
}

void Apu::write_byte(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::Apu)) {
        m_register_block1[address - memmap::ApuBegin] = value;
    }
    if (memmap::is_in(address, memmap::WavePattern)) {
        m_register_block2[address - memmap::WavePatternBegin] = value;
    }
    m_logger->debug("APU: Unhandled write at {:04X}", address);
}
