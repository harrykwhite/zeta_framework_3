#include <zf3_public.h>
#include "game.h"

int main() {
    ZF3GameUserInfo gameInfo = {0};
    gameInfo.init = game_init;
    gameInfo.tick = game_tick;
    gameInfo.cleanup = game_cleanup;
    gameInfo.windowInitWidth = 1280;
    gameInfo.windowInitHeight = 720;
    gameInfo.windowTitle = "Example ZF3 Game";

    zf3_run_game(&gameInfo);
}
