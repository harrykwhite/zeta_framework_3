#pragma once

#include <zf3_renderer.h>
#include <zf3_audio.h>
#include <zf3_rand.h>

namespace zf3 {
    // References to framework data provided to the user in their defined game functions.
    struct UserGameFuncData {
        Renderer& renderer;
        SoundSrcManager& sndSrcManager;
        MusicSrcManager& musicSrcManager;
    };

    using UserGameInit = bool (*)(const UserGameFuncData& data);
    using UserGameTick = bool (*)(const UserGameFuncData& data);
    using UserGameClean = void (*)();

    // Information provided by the user to the framework to start the game.
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
