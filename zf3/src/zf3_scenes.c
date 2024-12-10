#include <zf3.h>

#define SCENE_TYPE_LIMIT 256
#define SCENE_MEM_ARENA_SIZE ZF3_MEGABYTES(32)

static bool i_initialized;

static ZF3SceneTypeInfo i_sceneTypeInfos[SCENE_TYPE_LIMIT];
static int i_sceneTypeCnt;

static int i_activeSceneTypeIndex;
static ZF3MemArena i_activeSceneMemArena;
static ZF3ScenePtrs i_activeScenePtrs;

static void load_scene(const int sceneTypeIndex) {
    assert(!i_activeSceneMemArena.used);
    assert(sceneTypeIndex >= 0 && sceneTypeIndex < i_sceneTypeCnt);

    i_activeSceneTypeIndex = sceneTypeIndex;

    i_activeScenePtrs.spriteRenderer = zf3_mem_arena_push(&i_activeSceneMemArena, sizeof(ZF3SpriteRenderer));
    i_activeScenePtrs.userData = zf3_mem_arena_push(&i_activeSceneMemArena, i_sceneTypeInfos[sceneTypeIndex].userDataSize);

    i_sceneTypeInfos[sceneTypeIndex].init(&i_activeSceneMemArena, &i_activeScenePtrs);
}

static void unload_active_scene() {
    i_sceneTypeInfos[i_activeSceneTypeIndex].cleanup(&i_activeSceneMemArena, &i_activeScenePtrs);
    zf3_sprite_renderer_cleanup(i_activeScenePtrs.spriteRenderer);
    zf3_mem_arena_reset(&i_activeSceneMemArena);
}

void zf3_register_scene_types(const ZF3SceneTypeInfo* const typeInfos, const int typeCnt) {
    assert(!i_sceneTypeCnt); // The scene types should not have already been registered.
    assert(typeCnt > 0 && typeCnt <= SCENE_TYPE_LIMIT);

    memcpy(i_sceneTypeInfos, typeInfos, sizeof(*typeInfos) * typeCnt);
    i_sceneTypeCnt = typeCnt;
}

bool zf3_scene_system_init() {
    assert(i_sceneTypeCnt); // The scene types should have been registered.
    assert(!i_initialized);

    if (!zf3_mem_arena_init(&i_activeSceneMemArena, SCENE_MEM_ARENA_SIZE)) {
        return false;
    }

    load_scene(0);

    i_initialized = true;

    return true;
}

void zf3_scene_system_cleanup() {
    assert(i_initialized);

    unload_active_scene();
    zf3_mem_arena_cleanup(&i_activeSceneMemArena);
}

void zf3_proc_scene_tick() {
    int nextSceneTypeIndex = -1; // To potentially be modified in the subsequent tick function.

    i_sceneTypeInfos[i_activeSceneTypeIndex].tick(&i_activeSceneMemArena, &i_activeScenePtrs, &nextSceneTypeIndex);

    if (nextSceneTypeIndex != -1) {
        assert(nextSceneTypeIndex >= 0 && nextSceneTypeIndex < i_sceneTypeCnt);
        unload_active_scene();
        load_scene(nextSceneTypeIndex);
    }
}

void zf3_render_scene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    zf3_render_sprites(i_activeScenePtrs.spriteRenderer);
}
