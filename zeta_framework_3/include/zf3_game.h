#pragma once

#include <zf3_window.h>
#include <zf3_assets.h>
#include <zf3_rendering.h>
#include <zf3_rand.h>

namespace zf3 {

using UserGameInit = void (*)();
using UserGameTick = void (*)();
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

void run_game(const UserGameInfo& userInfo);

}
