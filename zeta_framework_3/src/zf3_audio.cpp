#include "zf3_audio.h"

namespace zf3 {
    static ALCdevice* i_alDevice = nullptr;
    static ALCcontext* i_alContext = nullptr;

    static void release_sound_src_by_index(SoundSrcManager* const manager, const int index) {
        assert(index >= 0 && index < gk_soundSrcLimit);
        assert(manager->alIDs[index]);

        alDeleteSources(1, &manager->alIDs[index]);
        manager->alIDs[index] = 0;
    }

    bool init_audio_system() {
        assert(!i_alDevice && !i_alContext);

        // Open a playback device for OpenAL.
        i_alDevice = alcOpenDevice(nullptr);

        if (!i_alDevice) {
            log_error("Failed to open a device for OpenAL.");
            return false;
        }

        // Create an OpenAL context.
        i_alContext = alcCreateContext(i_alDevice, nullptr);

        if (!i_alContext) {
            log_error("Failed to create an OpenAL context.");
            return false;
        }

        alcMakeContextCurrent(i_alContext);

        // Verify necessary extensions are supported.
        if (!alIsExtensionPresent("AL_EXT_FLOAT32")) {
            log_error("AL_EXT_FLOAT32 is not supported by your OpenAL implementation.");
            return false;
        }

        return true;
    }

    void clean_audio_system() {
        if (i_alContext) {
            alcDestroyContext(i_alContext);
            i_alContext = nullptr;
        }

        if (i_alDevice) {
            alcCloseDevice(i_alDevice);
            i_alDevice = nullptr;
        }
    }

    void clean_sound_srcs(SoundSrcManager* const manager) {
        for (int i = 0; i < gk_soundSrcLimit; ++i) {
            if (manager->alIDs[i]) {
                alDeleteSources(1, &manager->alIDs[i]);
            }
        }

        //memset(manager, 0, sizeof(*manager));
    }

    void handle_auto_release_sound_srcs(SoundSrcManager* const manager) {
        for (int i = 0; i < gk_soundSrcLimit; ++i) {
            if (!is_bit_active(manager->autoReleases, i)) {
                continue;
            }

            ALint srcState;
            alGetSourcei(manager->alIDs[i], AL_SOURCE_STATE, &srcState);

            if (srcState == AL_STOPPED) {
                release_sound_src_by_index(manager, i);
                deactivate_bit(manager->autoReleases, i);
            }
        }
    }

    SoundSrcID add_sound_src(SoundSrcManager* const manager, const int sndIndex, const Sounds& snds) {
        for (int i = 0; i < gk_soundSrcLimit; ++i) {
            if (!manager->alIDs[i]) {
                alGenSources(1, &manager->alIDs[i]);
                alSourcei(manager->alIDs[i], AL_BUFFER, snds.bufALIDs[sndIndex]);

                ++manager->versions[i];

                return {
                    .index = i,
                    .version = manager->versions[i]
                };
            }
        }

        assert(false);

        return {};
    }

    void remove_sound_src(SoundSrcManager* const manager, const SoundSrcID srcID) {
        assert(srcID.index >= 0 && srcID.index < gk_soundSrcLimit);
        assert(manager->versions[srcID.index] == srcID.version);
        assert(manager->alIDs[srcID.index]);

        release_sound_src_by_index(manager, srcID.index);
    }

    void play_sound_src(const SoundSrcManager* const manager, const SoundSrcID srcID, const Sounds& snds, const float gain, const float pitch) {
        assert(srcID.index >= 0 && srcID.index < gk_soundSrcLimit);
        assert(manager->versions[srcID.index] == srcID.version);
        assert(manager->alIDs[srcID.index]);

        alSourceRewind(manager->alIDs[srcID.index]); // Restart if already playing.
        alSourcef(manager->alIDs[srcID.index], AL_GAIN, gain);
        alSourcef(manager->alIDs[srcID.index], AL_PITCH, pitch);
        alSourcePlay(manager->alIDs[srcID.index]);
    }

    void add_and_play_sound_src(SoundSrcManager* const manager, const int sndIndex, const Sounds& snds, const float gain, const float pitch) {
        const SoundSrcID srcID = add_sound_src(manager, sndIndex, snds);
        play_sound_src(manager, srcID, snds, gain, pitch);
        activate_bit(manager->autoReleases, srcID.index); // No reference to this source is returned, so it needs to be automatically released once it is detected as finished.
    }
}
