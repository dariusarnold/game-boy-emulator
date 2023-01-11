#pragma once

#include "audiochannel.hpp"

class WaveChannel: public AudioChannel{
public:
    uint8_t get_sample() override;
};
