#pragma once

#include <span>
#include <cstdint>
#include "SDL_audio.h"


class Resampler {
    SDL_AudioStream* m_audio_stream;


public:
    Resampler();

    void queue_resample_data(std::span<uint8_t> data);
    [[nodiscard]] int available() const;
    int get_resampled_data(std::span<uint8_t> buffer);
};
