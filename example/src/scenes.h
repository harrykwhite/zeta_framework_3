#ifndef SCENES_H
#define SCENES_H

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
    ZF3Sprite* playerSprite;
} LevelTwoSceneData;

void level_one_init(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs);
void level_one_tick(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs, int* const nextSceneTypeIndex);
void level_one_cleanup(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs);

void level_two_init(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs);
void level_two_tick(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs, int* const nextSceneTypeIndex);
void level_two_cleanup(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs);

#endif
