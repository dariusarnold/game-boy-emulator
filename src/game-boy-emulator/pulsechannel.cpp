#include "pulsechannel.hpp"
#include "bitmanipulation.hpp"
#include <cmath>
#include <cassert>

namespace {
const std::array<std::array<int, 8>, 4> WAVE_DUTY_CYCLES{
    std::array{0, 1, 1, 1, 1, 1, 1, 1}, std::array{0, 1, 1, 1, 1, 1, 1, 0},
    std::array{0, 0, 1, 1, 0, 0, 1, 1}, std::array{0, 0, 0, 0, 0, 0, 1, 1}};
}

void PulseChannel::trigger() {
    // Triggering for volume envelope
    m_volume_sweep_counter = get_volume_sweep_pace();
    m_current_volume = get_volume();
    // Triggering for frequency sweep
    m_shadow_frequency = get_current_wavelength();
    m_freq_sweep_timer = get_wavelength_sweep_pace();
    if (m_freq_sweep_timer == 0) {
        m_freq_sweep_timer = 8;
    }
    m_sweep_enabled = get_wavelength_sweep_pace() > 0 || get_sweep_slope() > 0;
    if (get_sweep_slope() > 0) {
        if (get_current_wavelength() > 2047) {
            set_enabled(false);
        }
    }
    // Triggering for length timer
    if (get_length_timer() == 64) {
        set_length_timer(0);
    }
}


uint8_t PulseChannel::get_sample() {
    const auto& wave_duty = WAVE_DUTY_CYCLES[get_wave_duty()];
    return wave_duty[m_waveform_index] * m_current_volume;
}

unsigned PulseChannel::calculate_frequency() const {
    auto new_frequency = m_shadow_frequency >> get_sweep_slope();
    if (get_sweep_direction() == SweepDirection::Increase) {
        new_frequency = m_shadow_frequency + new_frequency;
    } else {
        new_frequency = m_shadow_frequency - new_frequency;
    }
    assert(new_frequency >= 0 && "Negative frequency in channel 1");
    return static_cast<unsigned>(new_frequency);
}


void PulseChannel::do_frequency_sweep() {
    if (m_freq_sweep_timer > 0) {
        m_freq_sweep_timer--;
    }
    if (m_freq_sweep_timer == 0) {
        m_freq_sweep_timer = get_wavelength_sweep_pace();
        if (m_freq_sweep_timer == 0) {
            m_freq_sweep_timer = 8;
        }

        if (m_sweep_enabled and get_wavelength_sweep_pace() > 0) {
            auto new_frequency = calculate_frequency();
            if (new_frequency > 2048) {
                set_enabled(false);
            }
            if (new_frequency <= 2047 && get_wavelength_sweep_pace() > 0) {
                set_wavelength(new_frequency);
                m_shadow_frequency = new_frequency;
                // For overflow check
                new_frequency = calculate_frequency();
                if (new_frequency > 2047) {
                    set_enabled(false);
                }
            }
        }
    }
}

void PulseChannel::do_envelope_sweep() {
    auto envelope_sweep_pace = get_volume_sweep_pace();
    if (envelope_sweep_pace == 0) {
        return;
    }
    if (m_volume_sweep_counter > 0) {
        m_volume_sweep_counter--;
    }
    if (m_volume_sweep_counter == 0) {
        m_volume_sweep_counter = get_volume_sweep_pace();
        // Actually do the sweep every pace ticks of the envelope
        if (bitmanip::is_bit_set(read_nrx2(), 3)) {
            // Increase volume envelope
            if (m_current_volume < 14) {
                m_current_volume++;
            }
        } else {
            // Decrease volume envelope
            if (m_current_volume > 0) {
                m_current_volume--;
            }
        }
    }
}

void PulseChannel::do_sound_length() {
    if (!is_length_enabled()) {
        return;
    }
    // Length timer ticks up at 256 Hz from the initial value.
    // When it reaches 64, the channel is turned off.
    auto length_timer = get_length_timer();
    length_timer++;
    set_length_timer(length_timer);
    if (length_timer == 64) {
        set_enabled(false);
    }
}

uint16_t PulseChannel::get_current_wavelength() const {
    auto high = read_nrx4() & 0b111;
    return bitmanip::word_from_bytes(high, read_nrx3());
}

uint8_t PulseChannel::get_wavelength_sweep_pace() const {
    return read_nrx0() & 0b1110000 >> 4;
}

PulseChannel::SweepDirection PulseChannel::get_sweep_direction() const {
    return bitmanip::is_bit_set(read_nrx0(), 3) ? SweepDirection::Decrease
                                                : SweepDirection::Increase;
}

uint8_t PulseChannel::get_sweep_slope() const {
    return read_nrx0() & 0b111;
}

void PulseChannel::set_wavelength(uint16_t wavelength) {
    auto low = bitmanip::get_low_byte(wavelength);
    set_nrx3(low);
    auto high = bitmanip::get_high_byte(wavelength) & 0b111;
    // Only overwrite the low 3 bits
    auto prev_value = read_nrx4();
    auto new_value = (prev_value & 0b11111000) | high;
    set_nrx4(new_value);
}

uint8_t PulseChannel::get_wave_duty() const {
    return (read_nrx1() & 0b11000000) >> 6;
}

void PulseChannel::set_nrx4(uint8_t value) {
    AudioChannel::set_nrx4(value);
    if (bitmanip::is_bit_set(value, 7)) {
        set_enabled(true);
        trigger();
    }
}

uint8_t PulseChannel::get_length_timer() const {
    return read_nrx1() & 0b111111;
}

void PulseChannel::set_length_timer(uint8_t length_timer) {
    auto prev_value = read_nrx1() & 0b11000000;
    auto new_value = prev_value | length_timer;
    set_nrx1(new_value);
}

void PulseChannel::tick_wave() {
    m_cycle_count++;
    if (m_cycle_count + get_current_wavelength() > 2048) {
        m_cycle_count = 0;
        m_waveform_index++;
        m_waveform_index %= 8;
    }
}

uint8_t PulseChannel::get_volume() const {
    return (read_nrx2() & 0b11110000) >> 4;
}

uint8_t PulseChannel::get_volume_sweep_pace() const {
    return read_nrx2() & 0b111;
}

bool PulseChannel::is_length_enabled() const {
    return bitmanip::is_bit_set(read_nrx4(), 6);
}
