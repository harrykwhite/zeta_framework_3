#pragma once

#include <zf3_window.h>
#include <zf3_assets.h>
#include <zf3_rendering.h>

namespace zf3 {

struct UserGameFuncData {
    const WindowMeta* const windowMeta;
    const Assets* const assets;
    Renderer* const renderer;
    Camera* const cam;
};

using UserGameInit = void (*)(const UserGameFuncData* const data);
using UserGameTick = void (*)(const UserGameFuncData* const data);
using UserGameCleanup = void (*)();

struct UserGameInfo {
    UserGameInit init;
    UserGameTick tick;
    UserGameCleanup cleanup;

    int initWindowWidth;
    int initWindowHeight;
    const char* windowTitle;
    bool windowResizable;
    bool hideCursor;
};

void run_game(const UserGameInfo* const userInfo);

}
