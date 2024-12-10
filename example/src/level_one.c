#include "scenes.h"

void level_one_init(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs) {
    LevelOneSceneData* const data = ptrs->userData;

    data->playerSprite = zf3_gen_sprites(ptrs->spriteRenderer, 1);
    data->playerSprite->texIndex = 0;
    data->playerSprite->alpha = 1.0f;
    data->playerSprite->size = (ZF3Vec2D) {64.0f, 64.0f};
}

void level_one_tick(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs, int* const nextSceneTypeIndex) {
    LevelOneSceneData* const data = ptrs->userData;

    if (zf3_is_key_down(ZF3_KEY_W)) {
        --data->playerSprite->pos.y;
    }

    if (zf3_is_key_down(ZF3_KEY_S)) {
        ++data->playerSprite->pos.y;
    }

    if (zf3_is_key_pressed(ZF3_KEY_SPACE)) {
        *nextSceneTypeIndex = LEVEL_TWO_SCENE;
    }
}

void level_one_cleanup(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs) {
    LevelOneSceneData* const data = ptrs->userData;
}
