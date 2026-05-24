#pragma once

#include <IR_Common.hpp>
#include <string>

namespace IR::Audio {

    bool Init();
    void Shutdown();

    void Update();

    enum class SoundType {
        Null,
        MP3,
        WAV,
        OGG,
        FLAC,
    };

    struct SoundFormat {
        int channels;
        int sampleRate;
        int bytesPerSample;
        int bitsPerSample;
        void* data;
    };

    void* LoadMP3(const void* data, Int64 size, SoundFormat& soundData);
    void* LoadWAV(const void* data, Int64 size, SoundFormat& soundData);
    void* LoadOGG(const void* data, Int64 size, SoundFormat& soundData);
    void* LoadFLAC(const void* data, Int64 size, SoundFormat& soundData);

    using SoundHandle = unsigned int;
    constexpr SoundHandle INVALID_SOUND_HANDLE = 0;

    // TODO: Fill in all the sound data and functionality
    // A sound is just the data for the sound, does not play it.
    class Sound {
    public:
        Sound(const void* data, Int64 size, SoundType type);
        ~Sound();

        Sound(const Sound&) = delete;
        Sound& operator=(const Sound&) = delete;
        Sound(Sound&&) = delete;
        Sound& operator=(Sound&&) = delete;

        SoundHandle GetHandle() const IRX_RETURN(m_iHandle);
        float GetDuration() const;

        void Play(struct SoundParams& params);

    private:
        const void* m_pSoundData = nullptr;
        SoundFormat m_Format;
        SoundType m_Type = SoundType::Null;
        SoundHandle m_iHandle = INVALID_SOUND_HANDLE;
    };

    Sound* LoadSound(const void* data, Int64 size, SoundType type);
    Sound* LoadSound(const std::string& filePath);

    struct SoundParams {
        float volume = 1.0f;
        float pitch = 1.0f;
        float gain = 1.0f;
        glm::vec3 position = glm::vec3(0.0f);
        bool loop = false;
        bool relative = false;
    };

    constexpr Int64 MAX_SOUND_STREAMS = 1 << 16;

    using SoundStream = unsigned int;
    constexpr SoundStream INVALID_SOUND_STREAM = -1;

    Int64 GetSoundStreamCount();
    Int64 GetSoundStreamCount(Sound* sound);

    SoundStream PlaySound(Sound* sound, const SoundParams& params);

    void StopSound(SoundStream stream);
    void StopSound(Sound* sound);
    void StopSound();

    void PauseSound(SoundStream stream);
    void ResumeSound(SoundStream stream);

    float GetSoundDuration(SoundStream stream);
    float GetRemainingDuration(SoundStream stream);
}
