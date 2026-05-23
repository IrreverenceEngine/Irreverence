#include <AL/al.h>
#include <AL/alc.h>

#include <IR_Audio.hpp>

namespace IR::Audio {

    static ALCdevice* s_pDevice = nullptr;
    static ALCcontext* s_pContext = nullptr;
    static SoundStream s_Streams[MAX_SOUND_STREAMS];

    bool Init()
    {
        s_pDevice = alcOpenDevice(nullptr);

        if (!s_pDevice) {
            return false;
        }

        s_pContext = alcCreateContext(s_pDevice, nullptr);

        if (!s_pContext) {
            alcCloseDevice(s_pDevice);
            return false;
        }

        alcMakeContextCurrent(s_pContext);

        alGenSources(MAX_SOUND_STREAMS, s_Streams);

        return true;
    }

    void Shutdown()
    {
        alDeleteSources(MAX_SOUND_STREAMS, s_Streams);

        if (s_pContext) {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(s_pContext);
        }

        if (s_pDevice) {
            alcCloseDevice(s_pDevice);
        }
    }

    static bool isUsedSoundStream(SoundStream stream)
    {
        int buffer;
        alGetSourcei(stream, AL_BUFFER, &buffer);
        int state;
        alGetSourcei(stream, AL_SOURCE_STATE, &state);

        return buffer != 0 && state != AL_PLAYING && state != AL_PAUSED;
    }

    Int64 GetSoundStreamCount()
    {
        Int64 count = 0;

        for (Int64 i = 0; i < MAX_SOUND_STREAMS; i++) {
            if (!isUsedSoundStream(s_Streams[i])) {
                continue;
            }

            ++count;
        }

        return count;
    }

    Int64 GetSoundStreamCount(Sound* sound)
    {
        Int64 count = 0;

        for (Int64 i = 0; i < MAX_SOUND_STREAMS; i++) {
            int buffer;
            alGetSourcei(s_Streams[i], AL_BUFFER, &buffer);

            if (buffer != sound->GetHandle()) {
                continue;
            }

            int state;
            alGetSourcei(s_Streams[i], AL_SOURCE_STATE, &state);

            if (state != AL_PLAYING && state != AL_PAUSED) {
                continue;
            }

            ++count;
        }

        return count;
    }

    static SoundStream getFreeSoundStream()
    {
        for (Int64 i = 0; i < MAX_SOUND_STREAMS; i++) {
            if (isUsedSoundStream(s_Streams[i])) {
                continue;
            }

            return s_Streams[i];
        }

        return INVALID_SOUND_STREAM;
    }

    SoundStream PlaySound(Sound* sound, const SoundParams& params)
    {
        SoundStream stream = getFreeSoundStream();

        if (stream < 0) {
            return INVALID_SOUND_STREAM;
        }

        if (sound->GetHandle() == INVALID_SOUND_HANDLE) {
            return INVALID_SOUND_STREAM;
        }

        alSourcei(stream, AL_BUFFER, sound->GetHandle());
        alSourcef(stream, AL_GAIN, params.gain);
        alSourcef(stream, AL_PITCH, params.pitch);
        alSource3f(stream, AL_POSITION, params.position.x, params.position.y, params.position.z);
        alSourcei(stream, AL_LOOPING, params.loop ? AL_TRUE : AL_FALSE);

        alSourcePlay(stream);

        return stream;
    }

    void StopSound(SoundStream stream)
    {
        if (stream < 0 || stream >= MAX_SOUND_STREAMS) {
            return;
        }

        if (!isUsedSoundStream(stream)) {
            return;
        }

        alSourceStop(stream);
        alSourcei(stream, AL_BUFFER, 0);
    }

    static bool isStreamSound(SoundStream stream, Sound* sound)
    {
        if (stream < 0 || stream >= MAX_SOUND_STREAMS) {
            return false;
        }

        int buffer;
        alGetSourcei(stream, AL_BUFFER, &buffer);

        return buffer == sound->GetHandle();
    }

    void StopSound(Sound* sound)
    {
        for (Int64 i = 0; i < MAX_SOUND_STREAMS; i++) {
            if (!isStreamSound(s_Streams[i], sound)) {
                continue;
            }

            StopSound(s_Streams[i]);
        }
    }

    void StopSound()
    {
        for (Int64 i = 0; i < MAX_SOUND_STREAMS; i++) {
            if (!isUsedSoundStream(s_Streams[i])) {
                continue;
            }

            StopSound(s_Streams[i]);
        }
    }

    void PauseSound(SoundStream stream)
    {
        if (!isUsedSoundStream(stream)) {
            return;
        }

        alSourcePause(stream);
    }

    void ResumeSound(SoundStream stream)
    {
        if (!isUsedSoundStream(stream)) {
            return;
        }

        alSourcePlay(stream);
    }

    float GetSoundDuration(SoundStream stream) IR_UNIMPLEMENTED
    float GetRemainingDuration(SoundStream stream) IR_UNIMPLEMENTED

}
