#include "resampler.hpp"
#include "constants.h"

#include "spdlog/spdlog.h"


Resampler::Resampler() {
    m_audio_stream
        = SDL_NewAudioStream(AUDIO_F32, 2, constants::CLOCK_SPEED_M, AUDIO_F32, 2, 44100);
    if (m_audio_stream == nullptr) {
        spdlog::error("Failed to create SDL_AudioStream: {}", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }
}

void Resampler::queue_resample_data(std::span<uint8_t> data) {
    auto res = SDL_AudioStreamPut(m_audio_stream, data.data(), data.size());
    if (res != 0) {
        spdlog::error("Failed to queue audio for resample: {}", SDL_GetError());
    }
}

int Resampler::available() const {
    return SDL_AudioStreamAvailable(m_audio_stream);
}

int Resampler::get_resampled_data(std::span<uint8_t> buffer) {
    return SDL_AudioStreamGet(m_audio_stream, buffer.data(), buffer.size());
}
