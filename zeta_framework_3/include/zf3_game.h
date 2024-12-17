#pragma once

#include <zf3_window.h>
#include <zf3_assets.h>
#include <zf3_rendering.h>
#include <zf3_audio.h>
#include <zf3_rand.h>

namespace zf3 {
    struct UserGameFuncData {
        const Window* window;
        const InputManager* inputManager;
        const Assets* assets;
        Renderer* renderer;
        SoundSrcManager* sndSrcManager;
        MusicSrcManager* musicSrcManager;
    };

    using UserGameInit = bool (*)(const UserGameFuncData* const data);
    using UserGameTick = bool (*)(const UserGameFuncData* const data);

    struct UserGameInfo {
        UserGameInit init;
        UserGameTick tick;

        int initWindowWidth;
        int initWindowHeight;
        const char* windowTitle;
        bool windowResizable;
        bool hideCursor;
    };

    void run_game(const UserGameInfo& userInfo);
}
