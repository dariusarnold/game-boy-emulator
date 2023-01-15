#pragma once

#include "audiochannel.hpp"
#include "clocktimer.hpp"
#include <cstddef>

/*
 * A square wave channel with wavelength sweep.
 */
class PulseChannel : public AudioChannel {
    bool m_dac_enabled = false;

    /*
     * Frequency/wavelength sweep
     */
    // After how many 128 Hz frequency sweep ticks should the wavelength change
    // Bit 4..6 of NR10
    [[nodiscard]] uint8_t get_wavelength_sweep_pace() const;
    enum class SweepDirection {
        Increase,
        Decrease,
    };
    // Bit 3 of NR10 giving the direction of the frequency sweep
    [[nodiscard]] SweepDirection get_sweep_direction() const;
    // Bits 0..2 of NR10, indicating how much the frequency should change
    [[nodiscard]] uint8_t get_sweep_slope() const;
    // Countdown of how many 128 Hz frequency sweep ticks occurred.
    uint8_t m_freq_sweep_timer = 0;
    bool m_sweep_enabled = false;
    uint16_t m_shadow_frequency = 0;
    [[nodiscard]] unsigned calculate_frequency() const;
    [[nodiscard]] uint16_t get_current_wavelength() const;
    void set_wavelength(uint16_t wavelength);

    // Bits 6-7 of NR11
    [[nodiscard]] uint8_t get_wave_duty() const;

    /*
     * Length timer
     */
    // Bits 0-5 of NR11
    [[nodiscard]] uint8_t get_length_timer() const;
    void set_length_timer(uint8_t length_timer);
    // Bit 6 of NR14
    [[nodiscard]] bool is_length_enabled() const;


    // Index into the 8 step square waveform
    uint8_t m_waveform_index = 0;
    size_t m_cycle_count = 0;



    void trigger();

    /*
     * Volume sweep
     */
    // Bits 4..7 if NR12
    [[nodiscard]] uint8_t get_volume() const;
    // Bits 0..2 fo NR12
    [[nodiscard]] uint8_t get_volume_sweep_pace() const;
    uint8_t m_volume_sweep_counter = 0;
    uint8_t m_current_volume = 0;

public:
    void tick_wave();
    uint8_t get_sample() override;

    void do_frequency_sweep();
    void do_envelope_sweep();
    void do_sound_length();

//    void set_nrx0(uint8_t value) override;
//    void set_nrx1(uint8_t value) override;
//    void set_nrx2(uint8_t value) override;
//    void set_nrx3(uint8_t value) override;
    void set_nrx4(uint8_t value) override;
//    uint8_t read_nrx0() const override;
//    uint8_t read_nrx1() const override;
//    uint8_t read_nrx2() const override;
//    uint8_t read_nrx3() const override;
//    uint8_t read_nrx4() const override;
};
