#pragma once

#include "audiochannel.hpp"

class NoiseChannel : public AudioChannel {
public:
    uint8_t get_sample() override;
};
