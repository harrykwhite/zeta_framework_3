#include <zf3.h>

enum SceneType {
    LEVEL_ONE_SCENE,
    LEVEL_TWO_SCENE,

    SCENE_TYPE_CNT
};

typedef struct {
    ZF3Sprite* playerSprite;
} LevelOneSceneData;

typedef struct {
    int a;
} LevelTwoSceneData;

void level_one_init(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs) {
    LevelOneSceneData* const data = ptrs->userData;

    data->playerSprite = zf3_gen_sprites(ptrs->spriteRenderer, 1);
    data->playerSprite->alpha = 1.0f;
    data->playerSprite->size = (ZF3Vec2D) {64.0f, 64.0f};
}

void level_one_tick(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs, int* const nextSceneTypeIndex) {
    LevelOneSceneData* const data = ptrs->userData;
}

void level_one_cleanup(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs) {
    LevelOneSceneData* const data = ptrs->userData;
}

void level_two_init(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs) {
}

void level_two_tick(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs) {
}

void level_two_cleanup(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs) {
}

int main() {
    // Register scene types.
    {
        ZF3SceneTypeInfo sceneTypeInfos[SCENE_TYPE_CNT] = {0};

        sceneTypeInfos[LEVEL_ONE_SCENE].init = level_one_init;
        sceneTypeInfos[LEVEL_ONE_SCENE].tick = level_one_tick;
        sceneTypeInfos[LEVEL_ONE_SCENE].cleanup = level_one_cleanup;
        sceneTypeInfos[LEVEL_ONE_SCENE].userDataSize = sizeof(LevelOneSceneData);

        sceneTypeInfos[LEVEL_TWO_SCENE].init = level_two_init;
        sceneTypeInfos[LEVEL_TWO_SCENE].tick = level_two_tick;
        sceneTypeInfos[LEVEL_TWO_SCENE].cleanup = level_two_cleanup;
        sceneTypeInfos[LEVEL_TWO_SCENE].userDataSize = sizeof(LevelTwoSceneData);

        zf3_register_scene_types(sceneTypeInfos, SCENE_TYPE_CNT);
    }

    // Run the game.
    ZF3GameInfo gameInfo = {0};
    gameInfo.windowInitWidth = 1280;
    gameInfo.windowInitHeight = 720;
    gameInfo.windowTitle = "Example ZF3 Game";

    zf3_run_game(&gameInfo);
}
