#include "audio.hpp"
#include "constants.h"
#include "emulator.hpp"
#include <SDL_audio.h>
#include "spdlog/spdlog.h"

namespace {
const int SDL_AUDIO_PLAYBACK = 0;
constexpr int BUFFER_SIZE = 4096;
} // namespace

Audio::Audio(Emulator& emulator) :
        m_resampler(AUDIO_F32SYS, AUDIO_F32SYS, constants::CLOCK_SPEED_M, 44100, 2, 2),
        m_emulator(emulator) {
    auto num_audio_devices = SDL_GetNumAudioDevices(0);
    std::vector<const char*> device_ids(static_cast<size_t>(num_audio_devices));
    for (auto i = 0; i < num_audio_devices; ++i) {
        // TODO Implement device selection by user
        const auto* name = SDL_GetAudioDeviceName(i, SDL_AUDIO_PLAYBACK);
        device_ids[static_cast<size_t>(i)] = name;
        spdlog::info("Detected audio device {}: {}", i, name);
    }

    SDL_AudioSpec audio_spec{};
    audio_spec.freq = 44100;
    audio_spec.format = AUDIO_F32;
    audio_spec.samples = BUFFER_SIZE;
    audio_spec.channels = 2;
    SDL_AudioSpec obtained{};
    spdlog::info("Using audio device {}", device_ids[0]);
    m_device_id = SDL_OpenAudioDevice(device_ids[0], SDL_AUDIO_PLAYBACK, &audio_spec, &obtained, 0);
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

namespace {
// Scale input value logarithmically to account for the perception of volume.
// A logarithmic scale gives the perception of a linear volume change.
float calc_volume_log(float volume) {
    return (std::exp(volume) - 1) / (std::exp(1) - 1);
}
}

void Audio::callback(SampleFrame sample) {
    m_resampler.submit_sample_data(sample);
    // Only submit to the actual audio playback if a good amount of samples are available since
    // queuing data involves locks on SDLs side.
    if (m_resampler.available_samples() >= BUFFER_SIZE) {
        auto resampled_data = m_resampler.get_resampled_data();
        auto volume = calc_volume_log(m_emulator.get_options().volume) * constants::FIXED_VOLUME_SCALE;
        std::for_each(resampled_data.begin(), resampled_data.end(), [volume](SampleFrame& sf) {
            sf.left *= volume;
            sf.right *= volume;
        });
        SDL_QueueAudio(m_device_id, resampled_data.data(), get_buffersize_bytes(resampled_data));
    }
}
