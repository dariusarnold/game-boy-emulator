#include "audio.hpp"
#include <SDL_audio.h>
#include "spdlog/spdlog.h"

namespace {
const int SDL_AUDIO_PLAYBACK = 0;
const char* SDL_AUDIO_DEFAULT_DEVICE = nullptr;
constexpr int BUFFER_SIZE = 4096;
} // namespace

Audio::Audio() {

    auto num_audio_devices = SDL_GetNumAudioDevices(0);
    for (auto i = 0; i < num_audio_devices; ++i) {
        // TODO Implement device selection by user
        const auto* name = SDL_GetAudioDeviceName(i, SDL_AUDIO_PLAYBACK);
        spdlog::info("Detected audio device {}: {}", i, name);
    }

    SDL_AudioSpec audio_spec{};
    audio_spec.freq = 44100;
    audio_spec.format = AUDIO_F32;
    audio_spec.samples = BUFFER_SIZE;
    audio_spec.channels = 2;
    SDL_AudioSpec obtained{};
    m_device_id = SDL_OpenAudioDevice(SDL_AUDIO_DEFAULT_DEVICE, SDL_AUDIO_PLAYBACK, &audio_spec,
                                      &obtained, 0);
    assert(obtained.freq == audio_spec.freq && "Audio device configuration mismatch");
    if (m_device_id == 0) {
        spdlog::error("Failed to open audio: {}", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }
    // Start playback on device
    SDL_PauseAudioDevice(m_device_id, 0);
}

Audio::~Audio() {
    SDL_CloseAudioDevice(m_device_id);
}

void Audio::callback(SampleFrame sample) {
    m_buffer.push_back(sample.left);
    m_buffer.push_back(sample.right);
    if (m_buffer.size() > BUFFER_SIZE) {
        m_resampler.queue_resample_data(get_buffer_as_span());
        // This needs the number of bytes in the buffer, not number of elements
        auto resampled_bytes = m_resampler.get_resampled_data(get_buffer_as_span());
        if (resampled_bytes == -1) {
            spdlog::error("Failed to get resampled bytes: {}", SDL_GetError());
            resampled_bytes = 0;
        }
        SDL_QueueAudio(m_device_id, m_buffer.data(), static_cast<Uint32>(resampled_bytes));
        m_buffer.clear();
    }
}

size_t Audio::get_buffersize_bytes() const {
    return m_buffer.size() * sizeof(decltype(m_buffer)::value_type);
}

std::span<uint8_t> Audio::get_buffer_as_span() {
    return std::span{reinterpret_cast<uint8_t*>(m_buffer.data()), get_buffersize_bytes()};
}
