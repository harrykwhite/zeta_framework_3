#pragma once

#include <zf3_window.h>
#include <zf3_assets.h>
#include <zf3_rendering.h>
#include <zf3_rand.h>

namespace zf3 {
    struct UserGameFuncData {
        const Window* window;
        const InputManager* inputManager;
        const Assets* assets;
        Renderer* renderer;
    };

    using UserGameInit = bool (*)(const UserGameFuncData& data);
    using UserGameTick = bool (*)(const UserGameFuncData& data);

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
