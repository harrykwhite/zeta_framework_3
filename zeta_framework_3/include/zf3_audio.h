#pragma once

#include <assert.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <zf3c_mem.h>
#include <zf3_misc.h>
#include <zf3_assets.h>

namespace zf3 {
    constexpr int gk_soundSrcLimit = 128;

    constexpr int gk_musicBufCnt = 4; // Number of buffers that can concurrently hold parts of a music source's sample data.
    constexpr int gk_musicBufSampleCnt = 44100;
    constexpr int gk_musicBufSize = sizeof(AudioSample) * gk_musicBufSampleCnt;
    constexpr int gk_musicSrcLimit = 16;

    struct AudioSrcID {
        int index;
        int version;
    };

    struct SoundSrcManager {
        ALID alIDs[gk_soundSrcLimit];
        int versions[gk_soundSrcLimit];
        StaticBitset<gk_soundSrcLimit> autoReleases; // Indicates which sources need to be automatically released when finished (due to not them not being referenced).
    };

    struct MusicSrc {
        int musicIndex;

        ALID alID;
        ALID bufALIDs[gk_musicBufCnt];

        FILE* fs;
        int fsBytesRead;
    };

    struct MusicSrcManager {
        MusicSrc srcs[gk_musicSrcLimit];
        StaticBitset<gk_musicSrcLimit> activity;
        int versions[gk_musicSrcLimit];
    };

    bool init_audio_system();
    void clean_audio_system();

    void clean_sound_srcs(SoundSrcManager& manager);
    void handle_auto_release_sound_srcs(SoundSrcManager& manager);
    AudioSrcID add_sound_src(SoundSrcManager& manager, const int sndIndex);
    void remove_sound_src(SoundSrcManager& manager, const AudioSrcID srcID);
    void play_sound_src(const SoundSrcManager& manager, const AudioSrcID srcID, const float gain = 1.0f, const float pitch = 1.0f);
    void add_and_play_sound_src(SoundSrcManager& manager, const int sndIndex, const float gain = 1.0f, const float pitch = 1.0f);

    void clean_music_srcs(MusicSrcManager& manager);
    bool refresh_music_src_bufs(MusicSrcManager& manager);
    AudioSrcID add_music_src(MusicSrcManager& manager, const int musicIndex);
    void remove_music_src(MusicSrcManager& manager, const AudioSrcID id);
    bool play_music_src(MusicSrcManager& manager, const AudioSrcID id, const float gain = 1.0f);
}
