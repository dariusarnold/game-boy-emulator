#include "apu.hpp"
#include "memorymap.hpp"
#include "bitmanipulation.hpp"
#include "emulator.hpp"
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
const uint16_t NR21_ADDRESS = 0xFF16;
const uint16_t NR22_ADDRESS = 0xFF17;
const uint16_t NR23_ADDRESS = 0xFF18;
const uint16_t NR24_ADDRESS = 0xFF19;
} // namespace

// Frame sequencer ticks every 8192 T cycle
Apu::Apu(Emulator* emulator) :
        m_logger(spdlog::get("")), m_frame_sequencer_timer(2048), m_emulator(emulator) {}

uint8_t Apu::read_byte(uint16_t address) {
    if (memmap::is_in(address, memmap::Apu)) {
        m_logger->trace("APU read {:04X}", address);
        switch (address) {
        case NR52_ADDRESS:
            return static_cast<int>(m_apu_enabled) << 7
                   | static_cast<int>(m_channel4.is_enabled()) << 3
                   | static_cast<int>(m_channel3.is_enabled()) << 2
                   | static_cast<int>(m_channel2.is_enabled()) << 1
                   | static_cast<int>(m_channel1.is_enabled());
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
        m_logger->trace("APU write {:04X} value {:02X}", address, value);
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
        case NR21_ADDRESS:
            m_channel2.set_nrx1(value);
            break;
        case NR22_ADDRESS:
            m_channel2.set_nrx2(value);
            break;
        case NR23_ADDRESS:
            m_channel2.set_nrx3(value);
            break;
        case NR24_ADDRESS:
            m_channel2.set_nrx4(value);
            break;
        default:
            m_logger->info("APU: Unhandled write at {:04X}", address);
            m_register_block1.at(address - memmap::ApuBegin) = value;
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
    if (m_frame_sequencer_timer.tick()) {
        // Frame sequencer stepping:
        // Step   Length Ctr  Vol Env     Sweep
        //---------------------------------------
        // 0      Clock       -           -
        // 1      -           -           -
        // 2      Clock       -           Clock
        // 3      -           -           -
        // 4      Clock       -           -
        // 5      -           -           -
        // 6      Clock       -           Clock
        // 7      -           Clock       -
        //---------------------------------------
        // Rate   256 Hz      64 Hz       128 Hz
        auto frame_sequencer_step = m_frame_sequencer_timer.get_trigger_count() % 8;
        switch (frame_sequencer_step) {
        case 0:
            m_channel1.do_sound_length();
            m_channel2.do_sound_length();
            break;
        case 1:
            break;
        case 2:
            m_channel1.do_sound_length();
            m_channel2.do_sound_length();
            // Only channel 1 has the frequency/wavelength sweep ability (not channel 2).
            m_channel1.do_frequency_sweep();
            break;
        case 3:
            break;
        case 4:
            m_channel1.do_sound_length();
            m_channel2.do_sound_length();
            break;
        case 5:
            break;
        case 6:
            m_channel1.do_sound_length();
            m_channel2.do_sound_length();
            m_channel1.do_frequency_sweep();
            break;
        case 7:
            m_channel1.do_envelope_sweep();
            m_channel2.do_envelope_sweep();
            break;
        default:
            assert(false && "There must be an error since this should be unreachable");
            break;
        }
    }
    m_channel1.tick_wave();
    m_channel2.tick_wave();
}


namespace {

float high_pass_impl(float& capacitor, float in, bool dacs_enabled) {
    float out = 0.;
    if (dacs_enabled) {
        out = in - capacitor;
        // Capacitor slowly charges to in via their difference
        capacitor = in - out * 0.999832011f;
    }
    return out;
}

float high_pass_left(float in, bool dacs_enabled) {
    static float capacitor = 0;
    return high_pass_impl(capacitor, in, dacs_enabled);
}

float high_pass_right(float in, bool dacs_enabled) {
    static float capacitor = 0;
    return high_pass_impl(capacitor, in, dacs_enabled);
}

SampleFrame high_pass(SampleFrame in) {
    in.left = high_pass_left(in.left, true);
    in.right = high_pass_right(in.right, true);
    return in;
}

const int CH4_LEFT = 7;
const int CH3_LEFT = 6;
const int CH2_LEFT = 5;
const int CH1_LEFT = 4;
const int CH4_RIGHT = 3;
const int CH3_RIGHT = 2;
const int CH2_RIGHT = 1;
const int CH1_RIGHT = 0;

} // namespace

SampleFrame Apu::get_sample() {
    if (!m_apu_enabled) {
        return {};
    }

    ChannelSamples samples;
    if (m_channel1.is_enabled()) {
        // Channel output is 0..15, DAC converts it to -1..1
        auto value_digital = m_channel1.get_sample();
        samples.ch1 = convert_dac(value_digital);
    }
    if (m_channel2.is_enabled()) {
        auto value_digital = m_channel2.get_sample();
        samples.ch2 = convert_dac(value_digital);
    }
    auto mixed_sample = mix(samples);
    mixed_sample.left *= get_left_output_volume();
    mixed_sample.right *= get_right_output_volume();
    mixed_sample = high_pass(mixed_sample);
    return mixed_sample;
}

float Apu::get_left_output_volume() const {
    // A value of 0 is treated as volume 1 and a value of 7 is treated as volume 8 (no reduction).
    return static_cast<float>(((m_master_volume & 0b01110000) >> 4) + 1);
}

float Apu::get_right_output_volume() const {
    return static_cast<float>((m_master_volume & 0b111) + 1);
}

float Apu::convert_dac(uint8_t value) {
    return (static_cast<float>(value) - (15.f / 2.f)) / 7.5f;
}

SampleFrame Apu::mix(const ChannelSamples& samples) {
    // Pan audio channel samples depending on NR51
    SampleFrame out;
    auto options = m_emulator->get_options();
    if (options.apu_channel1_enabled && bitmanip::is_bit_set(m_sound_panning, CH1_LEFT)) {
        out.left += samples.ch1;
    }
    if (options.apu_channel1_enabled && bitmanip::is_bit_set(m_sound_panning, CH1_RIGHT)) {
        out.right += samples.ch1;
    }
    if (options.apu_channel2_enabled && bitmanip::is_bit_set(m_sound_panning, CH2_LEFT)) {
        out.left += samples.ch2;
    }
    if (options.apu_channel2_enabled && bitmanip::is_bit_set(m_sound_panning, CH2_RIGHT)) {
        out.right += samples.ch2;
    }
    if (options.apu_channel3_enabled && bitmanip::is_bit_set(m_sound_panning, CH3_LEFT)) {
        out.left += samples.ch3;
    }
    if (options.apu_channel3_enabled && bitmanip::is_bit_set(m_sound_panning, CH3_RIGHT)) {
        out.right += samples.ch3;
    }
    if (options.apu_channel4_enabled && bitmanip::is_bit_set(m_sound_panning, CH4_LEFT)) {
        out.left += samples.ch4;
    }
    if (options.apu_channel4_enabled && bitmanip::is_bit_set(m_sound_panning, CH4_RIGHT)) {
        out.right += samples.ch4;
    }
    return out;
}
