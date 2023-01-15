#pragma once

namespace spdlog {
class logger;
}
#include "pulsechannel.hpp"
#include "wavechannel.hpp"
#include "noisechannel.hpp"
#include "clocktimer.hpp"
#include <memory>
#include <cstdint>
#include <array>

struct SampleFrame {
    float left = 0;
    float right = 0;
};

class Apu {
    std::shared_ptr<spdlog::logger> m_logger;

    // From 0xFF10-0xFF26
    std::array<uint8_t, 23> m_register_block1{};
    // From 0xFF30-0xFF3F
    std::array<uint8_t, 16> m_register_block2{};

    // Corresponds to value of the 0xFF26/NR52 register
    bool m_apu_enabled = false;
    // Corresponds to value of 0xFF25/NR51 register
    uint8_t m_sound_panning = 0;
    // Corresponds to value of 0xFF24/NR50
    uint8_t m_master_volume = 0;

    PulseChannel m_channel1;
    PulseChannel m_channel2;
    WaveChannel m_channel3;
    NoiseChannel m_channel4;

    ClockTimer m_frame_sequencer_timer;

    // Get right/left volume from NR50
    uint8_t get_left_output_volume() const;
    uint8_t get_right_output_volume() const;

    // Analog-Digital conversion of value in range 0..15 to value in range -1..1
    float convert_dac(uint8_t value);

public:
    Apu();

    uint8_t read_byte(uint16_t address);

    void write_byte(uint16_t address, uint8_t value);

    void cycle_elapsed_callback(size_t cycle_count_m);

    SampleFrame get_sample();
};
