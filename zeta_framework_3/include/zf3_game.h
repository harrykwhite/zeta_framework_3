#pragma once

#include <zf3_window.h>
#include <zf3_assets.h>
#include <zf3_rendering.h>
#include <zf3_audio.h>
#include <zf3_rand.h>

namespace zf3 {
    struct UserGameFuncData {
        Renderer& renderer;
        SoundSrcManager& sndSrcManager;
        MusicSrcManager& musicSrcManager;
    };

    using UserGameInit = bool (*)(const UserGameFuncData& data);
    using UserGameTick = bool (*)(const UserGameFuncData& data);
    using UserGameClean = void (*)();

    struct UserGameInfo {
        UserGameInit init;
        UserGameTick tick;
        UserGameClean clean;

        int initWindowWidth;
        int initWindowHeight;
        const char* windowTitle;
        bool windowResizable;
        bool hideCursor;
    };

    void start_game(const UserGameInfo& userInfo);
}
