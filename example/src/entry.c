#include <zf3_public.h>
#include "game.h"

int main() {
    const ZF3GameUserInfo gameInfo = {
        .init = game_init,
        .tick = game_tick,
        .cleanup = game_cleanup,
        .windowInitWidth = 1280,
        .windowInitHeight = 720,
        .windowTitle = "Example ZF3 Game",
        .windowResizable = true
    };

    zf3_run_game(&gameInfo);
}
