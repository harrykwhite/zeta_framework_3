#include <zf3_public.h>
#include "game.h"

int main() {
    const ZF3UserGameInfo gameInfo = {
        .init = init_game,
        .tick = run_game_tick,
        .cleanup = clean_game,
        .initWindowWidth = 1280,
        .initWindowHeight = 720,
        .windowTitle = "Example ZF3 Game",
        .windowResizable = true
    };

    zf3_run_game(&gameInfo);
}
