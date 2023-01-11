#pragma once

#include <cstdint>

class AudioChannel {

    bool m_enabled = false;

protected:
    // Registers of this channel
    uint8_t m_nrx0 = 0;
    uint8_t m_nrx1 = 0;
    uint8_t m_nrx2 = 0;
    uint8_t m_nrx3 = 0;
    uint8_t m_nrx4 = 0;

public:
    void set_enabled(bool enabled);
    [[nodiscard]] bool is_enabled() const;

    virtual uint8_t read_nrx0() const;
    virtual uint8_t read_nrx1() const;
    virtual uint8_t read_nrx2() const;
    virtual uint8_t read_nrx3() const;
    virtual uint8_t read_nrx4() const;
    virtual void set_nrx0(uint8_t value);
    virtual void set_nrx1(uint8_t value);
    virtual void set_nrx2(uint8_t value);
    virtual void set_nrx3(uint8_t value);
    virtual void set_nrx4(uint8_t value);

    // Generate a sample in range 0..15
    virtual uint8_t get_sample() = 0;

    virtual ~AudioChannel() = default;
};
