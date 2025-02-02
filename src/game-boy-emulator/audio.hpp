#pragma once

#include "apu.hpp"
#include "resampler.hpp"
#include "SDL_audio.h"
#include <vector>
class Emulator;

/**
 * Owner class for the SDL ressource AudioDeviceID.
 */
class AudioRessource {
    SDL_AudioDeviceID m_device_id{};

public:
    // Since opening the audio device can fail we need a way to check for a valid device
    [[nodiscard]] bool is_valid() const;
    [[nodiscard]] SDL_AudioDeviceID get() const;

    AudioRessource() = default;
    AudioRessource(const char* device, const SDL_AudioSpec& audio_spec);
    ~AudioRessource();
    // Since we handle a ressource in this class delete copy operators.
    AudioRessource(const AudioRessource&) = delete;
    AudioRessource& operator=(const AudioRessource&) = delete;
    AudioRessource(AudioRessource&& other) noexcept;
    AudioRessource& operator=(AudioRessource&& other) noexcept;
};

class Audio {
    AudioRessource m_audio_ressource;
    Resampler<SampleFrame> m_resampler;

    template <typename Container>
    static size_t get_buffersize_bytes(const Container& c) {
        return c.size() * sizeof(typename Container::value_type);
    }

    Emulator& m_emulator;

public:
    explicit Audio(Emulator& emulator);
    void callback(SampleFrame sample);
    [[nodiscard]] bool is_working() const;
};
