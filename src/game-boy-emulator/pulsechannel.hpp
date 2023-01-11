#pragma once

#include "audiochannel.hpp"

/*
 * A square wave channel with wavelength sweep.
 */
class PulseChannel : public AudioChannel {
    bool m_dac_enabled = false;

    // After how many 128 Hz frequency sweep ticks should the wavelength change
    // Bit 4..6 of NR10
    [[nodiscard]] uint8_t get_sweep_pace() const;
    enum class SweepDirection {
        Increase,
        Decrease,
    };
    // Bit 3 of NR10
    [[nodiscard]] SweepDirection get_sweep_direction() const;
    // Bits 0..2 of NR10
    [[nodiscard]] uint8_t get_sweep_slope() const;
    // How many 128 Hz frequency sweep ticks occured.
    uint8_t m_sweep_counter = 0;

    [[nodiscard]] uint16_t get_current_wavelength() const;
    void set_wavelength(uint16_t wavelength);

    // Bits 6-7 of NR11
    [[nodiscard]] uint8_t get_wave_duty() const;
    // Bits 0-5 of NR11
    [[nodiscard]] uint8_t get_length_timer() const;
    void set_length_timer(uint8_t length_timer);

public:
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
