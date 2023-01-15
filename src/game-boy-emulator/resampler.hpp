#pragma once

#include <span>
#include <cstdint>
#include "spdlog/spdlog.h"
#include "SDL_audio.h"


/*
 * Resample audio or change the number of channels.
 * Accepts data and stores them in an SDL managed buffer. Resampled data can be extracted.
 */
template <typename Sample>
class Resampler {
    SDL_AudioStream* m_audio_stream;

public:
    Resampler(int format_in, int format_out, size_t samplerate_in, size_t samplerate_out,
              int channels_in, int channels_out) {
        m_audio_stream = SDL_NewAudioStream(format_in, channels_in, samplerate_in, format_out,
                                            channels_out, samplerate_out);
        if (m_audio_stream == nullptr) {
            spdlog::error("Failed to create SDL_AudioStream: {}", SDL_GetError());
            std::exit(EXIT_FAILURE);
        }
    }
    ~Resampler() {
        SDL_FreeAudioStream(m_audio_stream);
    }
    // Since we manage a unique resource (the audio stream) delete all functions to prevent
    // duplication.
    Resampler(const Resampler&) = delete;
    Resampler(Resampler&&) = delete;
    Resampler& operator=(const Resampler&) = delete;
    Resampler& operator=(Resampler&&) = delete;

    // Add data to the resampler.
    // Data contains the samples to add.
    void submit_sample_data(std::span<Sample> data) {
        auto res = SDL_AudioStreamPut(m_audio_stream, data.data(), data.size_bytes());
        if (res != 0) {
            spdlog::error("Failed to queue audio for resample: {}", SDL_GetError());
        }
    }
    void submit_sample_data(Sample s) {
        auto res = SDL_AudioStreamPut(m_audio_stream, &s, sizeof(Sample));
        if (res != 0) {
            spdlog::error("Failed to queue audio for resample: {}", SDL_GetError());
        }
    }
    // Returns the number of samples available
    [[nodiscard]] size_t available_samples() const {
        return static_cast<size_t>(SDL_AudioStreamAvailable(m_audio_stream)) / sizeof(Sample);
    }
    // Write available resampled data into the buffer.
    // The buffer size should not exceed the number of samples available.
    int get_resampled_data(std::span<Sample> buffer) {
        return SDL_AudioStreamGet(m_audio_stream, buffer.data(), buffer.size_bytes());
    }
    std::vector<Sample> get_resampled_data() {
        const auto buffer_size = available_samples();
        std::vector<Sample> buffer(buffer_size);
        get_resampled_data(std::span{buffer});
        return buffer;
    }

    // Force flush all data from the stream to be available for output.
    void flush_queue() {
        SDL_AudioStreamFlush(m_audio_stream);
    }
};
