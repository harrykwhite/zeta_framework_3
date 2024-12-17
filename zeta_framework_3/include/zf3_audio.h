#pragma once

#include <assert.h>
#include <AL/alc.h>
#include <zf3_misc.h>
#include <zf3_assets.h>

namespace zf3 {
    constexpr int gk_soundSrcLimit = 128;

    struct SoundSrcID {
        int index;
        int version;
    };

    struct SoundSrcManager {
        ALID alIDs[gk_soundSrcLimit];
        int versions[gk_soundSrcLimit];
        Bitset<gk_soundSrcLimit> autoReleases; // Indicates which sources need to be automatically released when finished (due to not them not being referenced).
    };

    bool init_audio_system();
    void clean_audio_system();

    void clean_sound_srcs(SoundSrcManager* const manager);
    void handle_auto_release_sound_srcs(SoundSrcManager* const manager);
    SoundSrcID add_sound_src(SoundSrcManager* const manager, const int sndIndex, const Sounds& snds);
    void remove_sound_src(SoundSrcManager* const manager, const SoundSrcID srcID);
    void play_sound_src(const SoundSrcManager* const manager, const SoundSrcID srcID, const Sounds& snds, const float gain = 1.0f, const float pitch = 1.0f);
    void add_and_play_sound_src(SoundSrcManager* const manager, const int sndIndex, const Sounds& snds, const float gain = 1.0f, const float pitch = 1.0f);
}
