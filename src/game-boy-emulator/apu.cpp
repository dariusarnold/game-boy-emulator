#include "apu.hpp"
#include "memorymap.hpp"
#include "bitmanipulation.hpp"
#include "spdlog/spdlog.h"


namespace {
const uint16_t NR52_ADDRESS = 0xFF26;
const uint16_t NR51_ADDRESS = 0xFF25;
const uint16_t NR50_ADDRESS = 0xFF24;
const uint16_t NR10_ADDRESS = 0xFF10;
const uint16_t NR11_ADDRESS = 0xFF11;
const uint16_t NR12_ADDRESS = 0xFF12;
const uint16_t NR13_ADDRESS = 0xFF13;
const uint16_t NR14_ADDRESS = 0xFF14;

const int ENVELOPE_SWEEP_FREQ = 64;
const int SOUND_LENGTH_FREQ = 256;
const int CHA1_FREQ_SWEEP = 128;
} // namespace

Apu::Apu() : m_logger(spdlog::get("")) {}

uint8_t Apu::read_byte(uint16_t address) {
    if (memmap::is_in(address, memmap::Apu)) {
        m_logger->info("APU read {:04X}", address);
        switch (address) {
        case NR52_ADDRESS:
            return m_apu_enabled << 7 | m_channel4.is_enabled() << 3 | m_channel3.is_enabled() << 2
                   | m_channel2.is_enabled() << 1 | m_channel1.is_enabled();
        case NR51_ADDRESS:
            return m_sound_panning;
        case NR50_ADDRESS:
            return m_master_volume;
        case NR10_ADDRESS:
            return m_channel1.read_nrx0();
        case NR11_ADDRESS:
            return m_channel1.read_nrx1();
        case NR12_ADDRESS:
            return m_channel1.read_nrx2();
        case NR13_ADDRESS:
            return m_channel1.read_nrx3();
        case NR14_ADDRESS:
            return m_channel1.read_nrx4();
        default:
            return m_register_block1[address - memmap::ApuBegin];
        }
    }
    if (memmap::is_in(address, memmap::WavePattern)) {
        return m_register_block2[address - memmap::WavePatternBegin];
    }
    m_logger->debug("APU: Unhandled read at {:04X}", address);
    return 0xFF;
}

void Apu::write_byte(uint16_t address, uint8_t value) {
    if (memmap::is_in(address, memmap::Apu)) {
        m_logger->info("APU write {:04X} value {:02X}", address, value);
        switch (address) {
        case NR52_ADDRESS:
            // Only bit 7 (APU on/off) is writable
            m_apu_enabled = bitmanip::is_bit_set(value, 7);
            // TODO turning off the APU clears all APU registers and makes them read-only until
            // turned back on (except NR52).
            break;
        case NR51_ADDRESS:
            m_sound_panning = value;
            break;
        case NR50_ADDRESS:
            m_master_volume = value;
            break;
        case NR10_ADDRESS:
            m_channel1.set_nrx0(value);
            break;
        case NR11_ADDRESS:
            m_channel1.set_nrx1(value);
            break;
        case NR12_ADDRESS:
            m_channel1.set_nrx2(value);
            break;
        case NR13_ADDRESS:
            m_channel1.set_nrx3(value);
            break;
        case NR14_ADDRESS:
            m_channel1.set_nrx4(value);
            break;
        default:
            m_register_block1[address] = value;
            break;
        }
    } else if (memmap::is_in(address, memmap::WavePattern)) {
        m_register_block2[address - memmap::WavePatternBegin] = value;
    }
    m_logger->debug("APU: Unhandled write at {:04X}", address);
}

void Apu::cycle_elapsed_callback(size_t cycle_count_m) {
    // TODO Use DIV-APU as a clock source
    (void)cycle_count_m;
    if (cycle_count_m % CHA1_FREQ_SWEEP == 0) {
        m_channel1.do_frequency_sweep();
    }
    if (cycle_count_m % ENVELOPE_SWEEP_FREQ == 0) {
        m_channel1.do_envelope_sweep();
    }
    if (cycle_count_m % SOUND_LENGTH_FREQ == 0) {
        m_channel1.do_sound_length();
    }
}

SampleFrame Apu::get_sample() {
    SampleFrame out;
    if (!m_apu_enabled) {
        return out;
    }
    if (m_channel1.is_enabled()) {
        // Channel output is 0..15, DAC converts it to -1..1
        auto value_digital = m_channel1.get_sample();
        auto value_analog = convert_dac(value_digital);
        if (bitmanip::is_bit_set(m_sound_panning, 4)) {
            out.left += value_analog * get_left_output_volume();
        }
        if (bitmanip::is_bit_set(m_sound_panning, 0)) {
            out.right += value_analog * get_right_output_volume();
        }
    }
    return out;
}

uint8_t Apu::get_left_output_volume() const {
    // A value of 0 is treated as volume 1 and a value of 7 is treated as volume 8 (no reduction).
    return ((m_master_volume & 0b01110000) >> 4) + 1;
}

uint8_t Apu::get_right_output_volume() const {
    return (m_master_volume & 0b111) + 1;
}

float Apu::convert_dac(uint8_t value) {
    return (value - (15. / 2.)) / 7.5;
    ;
}
