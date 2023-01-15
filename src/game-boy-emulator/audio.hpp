#pragma once

#include "apu.hpp"
#include "resampler.hpp"
#include "SDL_audio.h"
#include <vector>


class Audio {
    SDL_AudioDeviceID m_device_id = 0;
    Resampler<SampleFrame> m_resampler;

    template <typename Container>
    static size_t get_buffersize_bytes(const Container& c) {
        return c.size() * sizeof(typename Container::value_type);
    }

public:
    Audio();
    ~Audio();

    void callback(SampleFrame sample);
};
