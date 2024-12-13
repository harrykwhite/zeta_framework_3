#include "scenes.h"

void world_init(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs) {
    WorldData* const data = ptrs->userData;
}

void world_tick(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs, int* const nextSceneTypeIndex) {
    WorldData* const data = ptrs->userData;
}

void world_cleanup(ZF3MemArena* const memArena, const ZF3ScenePtrs* const ptrs) {
    WorldData* const data = ptrs->userData;
}
