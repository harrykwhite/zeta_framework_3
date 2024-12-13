#ifndef SCENES_H
#define SCENES_H

#include <zf3_public.h>
#include "player.h"

enum SceneType {
    TITLE_SCREEN_SCENE,
    WORLD_SCENE,

    SCENE_TYPE_CNT
};

typedef struct {
    int a;
} TitleScreenData;

typedef struct {
    Player player;
} WorldData;

void title_screen_init(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs);
void title_screen_tick(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs, int* const nextSceneTypeIndex);
void title_screen_cleanup(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs);

void world_init(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs);
void world_tick(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs, int* const nextSceneTypeIndex);
void world_cleanup(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs);

#endif
