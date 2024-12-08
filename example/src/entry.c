#include <zf3.h>

int main() {
    ZF3GameInfo gameInfo = {0};
    gameInfo.windowInitWidth = 1280;
    gameInfo.windowInitHeight = 720;
    gameInfo.windowTitle = "Example ZF3 Game";

    zf3_run_game(&gameInfo);
}
