#include "audio.hpp"
#include "constants.h"
#include "emulator.hpp"
#include <SDL_audio.h>
#include "spdlog/spdlog.h"

namespace {
const int SDL_AUDIO_PLAYBACK = 0;
constexpr int BUFFER_SIZE = 4096;
} // namespace

void Audio::clear_queued_samples() {
    SDL_ClearQueuedAudio(m_audio_ressource.get());
    m_resampler.clear_audio_stream();
}

Audio::Audio(Emulator& emulator) :
        m_resampler(AUDIO_F32SYS, AUDIO_F32SYS, constants::CLOCK_SPEED_M, 44100, 2, 2),
        m_emulator(emulator) {
    SDL_AudioSpec audio_spec{};
    audio_spec.freq = 44100;
    audio_spec.format = AUDIO_F32;
    audio_spec.samples = BUFFER_SIZE;
    audio_spec.channels = 2;
    m_audio_ressource = AudioRessource(audio_spec);
}

namespace {
// Scale input value logarithmically to account for the perception of volume.
// A logarithmic scale gives the perception of a linear volume change.
float calc_volume_log(float volume) {
    return (std::exp(volume) - 1.f) / (std::exp(1.f) - 1.f);
}
} // namespace

void Audio::callback(SampleFrame sample) {
    m_resampler.submit_sample_data(sample);
    // Only submit to the actual audio playback if a good amount of samples are available since
    // queuing data involves locks on SDLs side.
    if (m_resampler.available_samples() >= BUFFER_SIZE) {
        auto resampled_data = m_resampler.get_resampled_data();
        auto volume
            = calc_volume_log(m_emulator.get_options().volume) * constants::FIXED_VOLUME_SCALE;
        std::for_each(resampled_data.begin(), resampled_data.end(), [volume](SampleFrame& sf) {
            sf.left *= volume;
            sf.right *= volume;
        });
        SDL_QueueAudio(m_audio_ressource.get(), resampled_data.data(),
                       static_cast<Uint32>(get_buffersize_bytes(resampled_data)));
    }
}

bool Audio::is_working() const {
    return m_audio_ressource.is_valid();
}

AudioRessource::AudioRessource(const SDL_AudioSpec& audio_spec) {
    SDL_AudioSpec obtained{};
    char* name_raw = nullptr;
    if (SDL_GetDefaultAudioInfo(&name_raw, &obtained, SDL_AUDIO_PLAYBACK) == 0) {
        std::unique_ptr<char, decltype(&SDL_free)> name{name_raw, &SDL_free};
        spdlog::info("Default audio device: {}", std::string_view(name.get()));
    }
    m_device_id = SDL_OpenAudioDevice(nullptr, SDL_AUDIO_PLAYBACK, &audio_spec, &obtained, 0);
    assert(obtained.freq == audio_spec.freq && "Audio device configuration mismatch");
    if (m_device_id == 0) {
        spdlog::error("Failed to open audio device: {}. No audio output.", SDL_GetError());
        return;
    }
    // Playback is always paused initially, unpause
    SDL_PauseAudioDevice(m_device_id, 0);
}

AudioRessource::~AudioRessource() {
    if (m_device_id != 0) {
        SDL_CloseAudioDevice(m_device_id);
    }
}

SDL_AudioDeviceID AudioRessource::get() const {
    assert(m_device_id != 0 && "Audio device not opened but requested");
    return m_device_id;
}

bool AudioRessource::is_valid() const {
    return m_device_id != 0;
}

AudioRessource::AudioRessource(AudioRessource&& other) noexcept : m_device_id(other.m_device_id) {
    other.m_device_id = 0;
}

AudioRessource& AudioRessource::operator=(AudioRessource&& other) noexcept {
    m_device_id = other.m_device_id;
    other.m_device_id = 0;
    return *this;
}
