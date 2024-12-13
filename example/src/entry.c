#include <zf3.h>
#include "scenes.h"

int main() {
    // Register scene types.
    {
        ZF3SceneTypeInfo sceneTypeInfos[SCENE_TYPE_CNT] = {0};

        sceneTypeInfos[TITLE_SCREEN_SCENE].init = title_screen_init;
        sceneTypeInfos[TITLE_SCREEN_SCENE].tick = title_screen_tick;
        sceneTypeInfos[TITLE_SCREEN_SCENE].cleanup = title_screen_cleanup;
        sceneTypeInfos[TITLE_SCREEN_SCENE].userDataSize = sizeof(TitleScreenData);

        sceneTypeInfos[WORLD_SCENE].init = world_init;
        sceneTypeInfos[WORLD_SCENE].tick = world_tick;
        sceneTypeInfos[WORLD_SCENE].cleanup = world_cleanup;
        sceneTypeInfos[WORLD_SCENE].userDataSize = sizeof(WorldData);

        zf3_register_scene_types(sceneTypeInfos, SCENE_TYPE_CNT);
    }

    // Run the game.
    ZF3GameInfo gameInfo = {0};
    gameInfo.windowInitWidth = 1280;
    gameInfo.windowInitHeight = 720;
    gameInfo.windowTitle = "Example ZF3 Game";

    zf3_run_game(&gameInfo);
}
