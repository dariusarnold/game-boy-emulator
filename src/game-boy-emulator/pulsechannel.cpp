#include "pulsechannel.hpp"
#include "bitmanipulation.hpp"
#include <cmath>

uint8_t PulseChannel::get_sample() {
    return 0;
}

void PulseChannel::do_frequency_sweep() {
    if (get_sweep_pace() == 0) {
        // When the pace is 0 iterations are disabled.
        return;
    }
    if (m_sweep_counter == get_sweep_pace()) {
        // Enough ticks occured to actually do a frequency sweep
        m_sweep_counter = 0;
        int wavelength = get_current_wavelength();
        auto n = get_sweep_slope();
        if (get_sweep_direction() == SweepDirection::Increase) {
            wavelength += wavelength / std::pow(2, n);
        } else {
            wavelength -= wavelength / std::pow(2, n);
        }
        if (wavelength > 0x7FF) {
            // If the wavelength would overflow, the channel is disabled
            set_enabled(false);
            return;
        }
        if (get_sweep_pace() != 0) {
            // For a slope of 0 iterations do nothing (except possibly turn of the channel if the
            // new wavelength would overflow).
            set_wavelength(wavelength);
        }
    }
    m_sweep_counter++;
}

void PulseChannel::do_envelope_sweep() {

}

void PulseChannel::do_sound_length() {
    // Length timer ticks up ad 256 Hz from the initial value.
    // When it reaches 64, the channel is turned off.
    auto length_timer = get_length_timer();
    length_timer++;
    set_length_timer(length_timer);
    if (length_timer == 64) {
        set_enabled(false);
    }
}

namespace {
//std::array<std::array<int, 8>, 4> WAVE_DUTY_CYCLES{
//    std::array{0, 1, 1, 1, 1, 1, 1, 1}, std::array{0, 1, 1, 1, 1, 1, 1, 0},
//    std::array{0, 0, 1, 1, 0, 0, 1, 1}, std::array{0, 0, 0, 0, 0, 0, 1, 1}};
}

uint16_t PulseChannel::get_current_wavelength() const {
    auto high = read_nrx4() & 0b111;
    return bitmanip::word_from_bytes(high, read_nrx3());
}

uint8_t PulseChannel::get_sweep_pace() const {
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
        // Todo Trigger channel
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
