#pragma once

#include "apu.hpp"
#include "resampler.hpp"
#include "SDL_audio.h"
#include <vector>



class Audio {
    SDL_AudioDeviceID m_device_id = 0;

    std::vector<float> m_buffer;

    Resampler m_resampler;

    size_t get_buffersize_bytes() const;
    std::span<uint8_t> get_buffer_as_span();
public:
    Audio();
    ~Audio();

    void callback(SampleFrame sample);

};
