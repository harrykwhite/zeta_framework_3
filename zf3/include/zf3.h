#ifndef ZF3_H
#define ZF3_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <zf3_common.h>

#define ZF3_GL_VERSION_MAJOR 4
#define ZF3_GL_VERSION_MINOR 1

//
// Window
//
typedef enum {
    ZF3_KEY_SPACE,

    ZF3_KEY_0,
    ZF3_KEY_1,
    ZF3_KEY_2,
    ZF3_KEY_3,
    ZF3_KEY_4,
    ZF3_KEY_5,
    ZF3_KEY_6,
    ZF3_KEY_7,
    ZF3_KEY_8,
    ZF3_KEY_9,

    ZF3_KEY_A,
    ZF3_KEY_B,
    ZF3_KEY_C,
    ZF3_KEY_D,
    ZF3_KEY_E,
    ZF3_KEY_F,
    ZF3_KEY_G,
    ZF3_KEY_H,
    ZF3_KEY_I,
    ZF3_KEY_J,
    ZF3_KEY_K,
    ZF3_KEY_L,
    ZF3_KEY_M,
    ZF3_KEY_N,
    ZF3_KEY_O,
    ZF3_KEY_P,
    ZF3_KEY_Q,
    ZF3_KEY_R,
    ZF3_KEY_S,
    ZF3_KEY_T,
    ZF3_KEY_U,
    ZF3_KEY_V,
    ZF3_KEY_W,
    ZF3_KEY_X,
    ZF3_KEY_Y,
    ZF3_KEY_Z,

    ZF3_KEY_ESCAPE,
    ZF3_KEY_ENTER,
    ZF3_KEY_TAB,

    ZF3_KEY_RIGHT,
    ZF3_KEY_LEFT,
    ZF3_KEY_DOWN,
    ZF3_KEY_UP,

    ZF3_KEY_F1,
    ZF3_KEY_F2,
    ZF3_KEY_F3,
    ZF3_KEY_F4,
    ZF3_KEY_F5,
    ZF3_KEY_F6,
    ZF3_KEY_F7,
    ZF3_KEY_F8,
    ZF3_KEY_F9,
    ZF3_KEY_F10,
    ZF3_KEY_F11,
    ZF3_KEY_F12,

    ZF3_KEY_LEFT_SHIFT,
    ZF3_KEY_LEFT_CONTROL,
    ZF3_KEY_LEFT_ALT,

    ZF3_KEY_CODE_CNT
} ZF3KeyCode;

typedef enum {
    ZF3_MOUSE_BUTTON_LEFT,
    ZF3_MOUSE_BUTTON_RIGHT,
    ZF3_MOUSE_BUTTON_MID,

    ZF3_MOUSE_BUTTON_CODE_CNT
} ZF3MouseButtonCode;

bool zf3_window_init(const int width, const int height, const char* const title, const bool resizable);
void zf3_window_cleanup();
void zf3_show_window();
bool zf3_should_window_close();
void zf3_swap_buffers();

void zf3_save_input_state();

bool zf3_is_key_down(const ZF3KeyCode keyCode);
bool zf3_is_key_pressed(const ZF3KeyCode keyCode);
bool zf3_is_key_released(const ZF3KeyCode keyCode);
bool zf3_is_mouse_button_down(const ZF3MouseButtonCode buttonCode);
bool zf3_is_mouse_button_pressed(const ZF3MouseButtonCode buttonCode);
bool zf3_is_mouse_button_released(const ZF3MouseButtonCode buttonCode);

//
// Assets
//
#define ZF3_TEX_LIMIT 256

typedef struct {
    int texCnt;

    GLuint texGLIDs[ZF3_TEX_LIMIT];
    ZF3Vec2DInt texSizes[ZF3_TEX_LIMIT];
} ZF3Assets;

typedef struct {
    GLuint spriteGLID;
    int spriteViewUniLoc;
    int spriteProjUniLoc;
    int spritePosUniLoc;
    int spriteSizeUniLoc;
    int spriteRotUniLoc;
    int spriteAlphaUniLoc;
    int spriteTexUniLoc;
} ZF3ShaderProgs;

bool zf3_load_assets(ZF3Assets* const assets, ZF3MemArena* const scratchSpace);
void zf3_unload_assets(ZF3Assets* const assets);

void zf3_load_shader_progs(ZF3ShaderProgs* const shaderProgs);
void zf3_unload_shader_progs(ZF3ShaderProgs* const shaderProgs);

//
// Rendering
//
#define ZF3_SPRITE_LIMIT 8192

typedef struct {
    ZF3Vec2D pos;
    ZF3Vec2D size;
    float rot;
    float alpha;
    int texIndex;
} ZF3Sprite;

typedef struct {
    GLuint spriteVertArrayGLIDs[ZF3_SPRITE_LIMIT];
    GLuint spriteVertBufGLIDs[ZF3_SPRITE_LIMIT];
    GLuint spriteElemBufGLIDs[ZF3_SPRITE_LIMIT];
    ZF3Sprite sprites[ZF3_SPRITE_LIMIT];
    int numSpritesGenerated;
} ZF3SpriteRenderer;

void zf3_sprite_renderer_cleanup(ZF3SpriteRenderer* const renderer);
ZF3Sprite* zf3_gen_sprites(ZF3SpriteRenderer* const renderer, const int spriteCnt);
void zf3_render_sprites(const ZF3SpriteRenderer* const renderer, const ZF3ShaderProgs* const shaderProgs);

//
// Scenes
//
typedef struct {
    ZF3SpriteRenderer* spriteRenderer;
    void* userData;
} ZF3ScenePtrs;

typedef void (*ZF3SceneInit)(ZF3MemArena* memArena, const ZF3ScenePtrs* ptrs);
typedef void (*ZF3SceneTick)(ZF3MemArena* memArena, const ZF3ScenePtrs* ptrs, int* const nextSceneTypeIndex);
typedef void (*ZF3SceneCleanup)(ZF3MemArena* memArena, const ZF3ScenePtrs* ptrs);

typedef struct {
    ZF3SceneInit init;
    ZF3SceneTick tick;
    ZF3SceneCleanup cleanup;

    int userDataSize;
} ZF3SceneTypeInfo;

void zf3_register_scene_types(const ZF3SceneTypeInfo* const typeInfos, const int typeCnt);
bool zf3_scene_system_init();
void zf3_scene_system_cleanup();
void zf3_proc_scene_tick();
void zf3_render_scene(const ZF3ShaderProgs* const shaderProgs);

//
// Game
//
typedef struct {
    int windowInitWidth;
    int windowInitHeight;
    const char* windowTitle;
    bool windowResizable;
} ZF3GameInfo;

void zf3_run_game(const ZF3GameInfo* const gameInfo);
const ZF3Assets* zf3_get_assets();

//
// Utilities
//
inline bool is_bit_active(const ZF3Byte* const bytes, const int bitIndex) {
    assert(bytes);
    assert(bitIndex >= 0);

    return bytes[bitIndex / 8] & (1 << (bitIndex % 8));
}

inline void activate_bit(ZF3Byte* const bytes, const int bitIndex) {
    assert(bytes);
    assert(bitIndex >= 0);

    bytes[bitIndex / 8] |= 1 << (bitIndex % 8);
}

inline void deactivate_bit(ZF3Byte* const bytes, const int bitIndex) {
    assert(bytes);
    assert(bitIndex >= 0);

    bytes[bitIndex / 8] &= ~(1 << (bitIndex % 8));
}

#endif
