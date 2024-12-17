#pragma once

#include <assert.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <zf3_misc.h>
#include <zf3_assets.h>

namespace zf3 {
    constexpr int gk_soundSrcLimit = 128;

    constexpr int gk_musicBufCnt = 4;
    constexpr int gk_musicBufSampleCnt = 44100;
    constexpr int gk_musicBufSize = sizeof(AudioSample) * gk_musicBufSampleCnt;
    constexpr int gk_musicSrcLimit = 16;

    struct SoundSrcID {
        int index;
        int version;
    };

    struct SoundSrcManager {
        ALID alIDs[gk_soundSrcLimit];
        int versions[gk_soundSrcLimit];
        Bitset<gk_soundSrcLimit> autoReleases; // Indicates which sources need to be automatically released when finished (due to not them not being referenced).
    };

    struct MusicSrc {
        int musicIndex;

        ALID alID;
        ALID bufALIDs[gk_musicBufCnt];

        FILE* fs;
        int fsBytesRead;
    };

    struct MusicSrcID {
        int index;
        int version;
    };

    struct MusicSrcManager {
        MusicSrc srcs[gk_musicSrcLimit];
        Bitset<gk_musicSrcLimit> activity;
        int versions[gk_musicSrcLimit];
    };

    bool init_audio_system();
    void clean_audio_system();

    void clean_sound_srcs(SoundSrcManager* const manager);
    void handle_auto_release_sound_srcs(SoundSrcManager* const manager);
    SoundSrcID add_sound_src(SoundSrcManager* const manager, const int sndIndex);
    void remove_sound_src(SoundSrcManager* const manager, const SoundSrcID srcID);
    void play_sound_src(const SoundSrcManager* const manager, const SoundSrcID srcID, const float gain = 1.0f, const float pitch = 1.0f);
    void add_and_play_sound_src(SoundSrcManager* const manager, const int sndIndex, const float gain = 1.0f, const float pitch = 1.0f);

    void clean_music_srcs(MusicSrcManager* const manager);
    bool refresh_music_src_bufs(MusicSrcManager* const manager);
    MusicSrcID add_music_src(MusicSrcManager* const manager, const int musicIndex);
    void remove_music_src(MusicSrcManager* const manager, const MusicSrcID id);
    bool play_music_src(MusicSrcManager* const manager, const MusicSrcID id, const float gain = 1.0f);
}
