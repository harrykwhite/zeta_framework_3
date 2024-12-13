#ifndef ZF3_PUBLIC_H
#define ZF3_PUBLIC_H

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <zf3c.h>

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

ZF3Vec2DInt zf3_get_window_size();

bool zf3_is_key_down(const ZF3KeyCode keyCode);
bool zf3_is_key_pressed(const ZF3KeyCode keyCode);
bool zf3_is_key_released(const ZF3KeyCode keyCode);
bool zf3_is_mouse_button_down(const ZF3MouseButtonCode buttonCode);
bool zf3_is_mouse_button_pressed(const ZF3MouseButtonCode buttonCode);
bool zf3_is_mouse_button_released(const ZF3MouseButtonCode buttonCode);

//
// Assets
//
#define ZF3_SPRITE_QUAD_SHADER_PROG_VERT_CNT 11

#define ZF3_TEX_LIMIT 256

typedef struct {
    int texCnt;

    GLuint texGLIDs[ZF3_TEX_LIMIT];
    ZF3Vec2DInt texSizes[ZF3_TEX_LIMIT];
} ZF3Assets;

typedef struct {
    GLuint spriteQuadGLID;
    int spriteQuadProjUniLoc;
    int spriteQuadViewUniLoc;
    int spriteQuadTexturesUniLoc;
} ZF3ShaderProgs;

const ZF3Assets* zf3_get_assets();
const ZF3ShaderProgs* zf3_get_shader_progs();

//
// Rendering
//
#define ZF3_SPRITE_BATCH_LIMIT 256
#define ZF3_SPRITE_BATCH_SLOT_LIMIT 4096
#define ZF3_SPRITE_BATCH_SLOT_VERT_CNT ZF3_SPRITE_QUAD_SHADER_PROG_VERT_CNT * 4
#define ZF3_TEX_UNIT_LIMIT 16

typedef struct {
    GLuint vertArrayGLID;
    GLuint vertBufGLID;
    GLuint elemBufGLID;
} ZF3SpriteBatchGLIDs;

typedef struct {
    int slotsUsed;

    int texUnitTexIDs[ZF3_TEX_UNIT_LIMIT];
    int texUnitsInUse;
} ZF3SpriteBatchTransData;

typedef struct {
    int texIndex;
    ZF3Vec2D pos;
    ZF3Rect srcRect;
    ZF3Vec2D origin;
    float rot;
    ZF3Vec2D scale;
    float alpha;
} ZF3SpriteBatchWriteData;

typedef struct {
    ZF3SpriteBatchGLIDs spriteBatchGLIDs[ZF3_SPRITE_BATCH_LIMIT];
    ZF3SpriteBatchTransData spriteBatchTransDatas[ZF3_SPRITE_BATCH_LIMIT]; // Cleared on emptying.
    int spriteBatchesFilled; // Reset on emptying.
    int spriteBatchCnt;
} ZF3Renderer;

void zf3_write_to_sprite_batch(ZF3Renderer* const renderer, const ZF3SpriteBatchWriteData* const writeData);

//
// Scenes
//
typedef struct {
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

//
// Utilities
//
inline bool zf3_is_bit_active(const ZF3Byte* const bytes, const int bitIndex) {
    assert(bytes);
    assert(bitIndex >= 0);

    return bytes[bitIndex / 8] & (1 << (bitIndex % 8));
}

inline void zf3_activate_bit(ZF3Byte* const bytes, const int bitIndex) {
    assert(bytes);
    assert(bitIndex >= 0);

    bytes[bitIndex / 8] |= 1 << (bitIndex % 8);
}

inline void zf3_deactivate_bit(ZF3Byte* const bytes, const int bitIndex) {
    assert(bytes);
    assert(bitIndex >= 0);

    bytes[bitIndex / 8] &= ~(1 << (bitIndex % 8));
}

inline void zf3_clear_bits(ZF3Byte* const bytes, const int bitCnt) {
    assert(bytes);
    assert(bitCnt > 0);
    memset(bytes, 0, ZF3_BITS_TO_BYTES(bitCnt));
}

inline int zf3_get_first_inactive_bit_index(const ZF3Byte* const bytes, const int bitCnt) {
    assert(bytes);
    assert(bitCnt > 0);

    for (int i = 0; i < bitCnt; ++i) {
        if (!zf3_is_bit_active(bytes, i)) {
            return i;
        }
    }

    return -1;
}

inline bool zf3_are_all_bits_active(const ZF3Byte* const bytes, const int bitCnt) {
    assert(bytes);
    assert(bitCnt > 0);

    for (int i = 0; i < bitCnt; ++i) {
        if (!zf3_is_bit_active(bytes, i)) {
            return false;
        }
    }

    return true;
}

#endif
