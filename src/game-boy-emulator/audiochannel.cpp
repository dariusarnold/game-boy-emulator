#include "audiochannel.hpp"

bool AudioChannel::is_enabled() const {
    return m_enabled;
}

void AudioChannel::set_enabled(bool enabled) {
    m_enabled = enabled;
}

uint8_t AudioChannel::read_nrx0() const {
    return m_nrx0;
}

uint8_t AudioChannel::read_nrx1() const {
    return m_nrx1;
}

uint8_t AudioChannel::read_nrx2() const {
    return m_nrx2;
}

uint8_t AudioChannel::read_nrx3() const {
    return m_nrx3;
}

uint8_t AudioChannel::read_nrx4() const {
    return m_nrx4;
}

void AudioChannel::set_nrx0(uint8_t value) {
    m_nrx0 = value;
}

void AudioChannel::set_nrx1(uint8_t value) {
    m_nrx1 = value;
}

void AudioChannel::set_nrx2(uint8_t value) {
    m_nrx2 = value;
}

void AudioChannel::set_nrx3(uint8_t value) {
    m_nrx3 = value;
}

void AudioChannel::set_nrx4(uint8_t value) {
    m_nrx4 = value;
}
