#pragma once

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <zf3c.h>

namespace zf3 {

//
// Window
//
using KeysDownBitset = unsigned long long;
using MouseButtonsDownBitset = unsigned char;

enum KeyCode {
    INVALID_KEY_CODE = -1,

    KEY_SPACE,

    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,

    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,

    KEY_ESCAPE,
    KEY_ENTER,
    KEY_TAB,

    KEY_RIGHT,
    KEY_LEFT,
    KEY_DOWN,
    KEY_UP,

    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,

    KEY_LEFT_SHIFT,
    KEY_LEFT_CONTROL,
    KEY_LEFT_ALT,

    NUM_KEY_CODES
};

enum MouseButtonCode {
    INVALID_MOUSE_BUTTON_CODE = -1,

    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_MID,

    NUM_MOUSE_BUTTON_CODES
};

struct InputState {
    KeysDownBitset keysDownBits;
    MouseButtonsDownBitset mouseButtonsDownBits;
    Vec2D mousePos;
};

struct WindowMeta {
    Vec2DInt size;
    InputState inputState;
    InputState inputStateSaved;
};

inline bool is_key_down(const KeyCode keyCode, const WindowMeta* const windowMeta) {
    return windowMeta->inputState.keysDownBits & ((KeysDownBitset)1 << keyCode);
}

inline bool is_key_pressed(const KeyCode keyCode, const WindowMeta* const windowMeta) {
    const KeysDownBitset keyBit = (KeysDownBitset)1 << keyCode;
    return (windowMeta->inputState.keysDownBits & keyBit) && !(windowMeta->inputStateSaved.keysDownBits & keyBit);
}

inline bool is_key_released(const KeyCode keyCode, const WindowMeta* const windowMeta) {
    const KeysDownBitset keyBit = (KeysDownBitset)1 << keyCode;
    return !(windowMeta->inputState.keysDownBits & keyBit) && (windowMeta->inputStateSaved.keysDownBits & keyBit);
}

inline bool is_mouse_button_down(const MouseButtonCode buttonCode, const WindowMeta* const windowMeta) {
    return windowMeta->inputState.mouseButtonsDownBits & ((MouseButtonsDownBitset)1 << buttonCode);
}

inline bool is_mouse_button_pressed(const MouseButtonCode buttonCode, const WindowMeta* const windowMeta) {
    const MouseButtonsDownBitset buttonBit = (MouseButtonsDownBitset)1 << buttonCode;
    return (windowMeta->inputState.mouseButtonsDownBits & buttonBit) && !(windowMeta->inputStateSaved.mouseButtonsDownBits & buttonBit);
}

inline bool is_mouse_button_released(const MouseButtonCode buttonCode, const WindowMeta* const windowMeta) {
    const MouseButtonsDownBitset buttonBit = (MouseButtonsDownBitset)1 << buttonCode;
    return !(windowMeta->inputState.mouseButtonsDownBits & buttonBit) && (windowMeta->inputStateSaved.mouseButtonsDownBits & buttonBit);
}

//
// Assets
//
constexpr int gk_spriteQuadShaderProgVertCnt = 11;

constexpr int gk_texLimit = 256;

struct Assets {
    int texCnt;
    GLuint texGLIDs[gk_texLimit];
    Vec2DInt texSizes[gk_texLimit];
};

struct ShaderProgs {
    GLuint spriteQuadGLID;
    int spriteQuadProjUniLoc;
    int spriteQuadViewUniLoc;
    int spriteQuadTexturesUniLoc;
};

//
// Rendering
//
constexpr int gk_renderLayerLimit = 32;
constexpr int gk_renderLayerSpriteBatchLimit = 256;
constexpr int gk_spriteBatchSlotLimit = 4096;
constexpr int gk_spriteBatchSlotVertCnt = gk_spriteQuadShaderProgVertCnt * 4;
constexpr int gk_texUnitLimit = 16; // This is the minimum guaranteed by OpenGL. For now, we don't consider any higher than this.

struct SpriteBatchGLIDs {
    GLuint vertArrayGLID;
    GLuint vertBufGLID;
    GLuint elemBufGLID;
};

struct SpriteBatchTransData {
    int slotsUsed;
    int texUnitTexIDs[gk_texUnitLimit];
    int texUnitsInUse;
};

struct SpriteBatchWriteData {
    int texIndex;
    Vec2D pos;
    Rect srcRect;
    Vec2D origin;
    float rot;
    Vec2D scale;
    float alpha;
};

struct RenderLayer {
    SpriteBatchGLIDs spriteBatchGLIDs[gk_renderLayerSpriteBatchLimit];
    SpriteBatchTransData spriteBatchTransDatas[gk_renderLayerSpriteBatchLimit]; // Cleared on emptying.
    int spriteBatchesFilled; // Cleared on emptying.
    int spriteBatchCnt;
};

struct Renderer {
    RenderLayer layers[gk_renderLayerLimit];
    int layerCnt;
    int camLayerCnt; // Layers 0 through to this number are drawn with a camera view matrix.
};

struct Camera {
    Vec2D pos;
    float scale;
};

void load_render_layers(Renderer* const renderer, const int layerCnt, const int camLayerCnt);
void write_to_sprite_batch(Renderer* const renderer, const int layerIndex, const SpriteBatchWriteData* const writeData, const Assets* const assets);

inline Vec2D conv_camera_to_screen_pos(const Vec2D pos, const Camera* const cam, const WindowMeta* const windowMeta) {
    return Vec2D {
        .x = ((pos.x - cam->pos.x) * cam->scale) + (windowMeta->size.x / 2.0f),
        .y = ((pos.y - cam->pos.y) * cam->scale) + (windowMeta->size.y / 2.0f)
    };
}

inline Vec2D conv_screen_to_camera_pos(const Vec2D pos, const Camera* const cam, const WindowMeta* const windowMeta) {
    return Vec2D {
        .x = ((pos.x - (windowMeta->size.x / 2.0f)) / cam->scale) + cam->pos.x,
        .y = ((pos.y - (windowMeta->size.y / 2.0f)) / cam->scale) + cam->pos.y
    };
}

//
// Game
//
struct UserGameFuncData {
    const WindowMeta* const windowMeta;
    const Assets* const assets;
    Renderer* const renderer;
    Camera* const cam;
};

using UserGameInit = void (*)(const UserGameFuncData* const data);
using UserGameTick = void (*)(const UserGameFuncData* const data);
using UserGameCleanup = void (*)();

struct UserGameInfo {
    UserGameInit init;
    UserGameTick tick;
    UserGameCleanup cleanup;

    int initWindowWidth;
    int initWindowHeight;
    const char* windowTitle;
    bool windowResizable;
};

void run_game(const UserGameInfo* const userInfo);

//
// Utilities
//
inline bool is_bit_active(const Byte* const bytes, const int bitIndex) {
    assert(bytes);
    assert(bitIndex >= 0);

    return bytes[bitIndex / 8] & (1 << (bitIndex % 8));
}

inline void activate_bit(Byte* const bytes, const int bitIndex) {
    assert(bytes);
    assert(bitIndex >= 0);

    bytes[bitIndex / 8] |= 1 << (bitIndex % 8);
}

inline void deactivate_bit(Byte* const bytes, const int bitIndex) {
    assert(bytes);
    assert(bitIndex >= 0);

    bytes[bitIndex / 8] &= ~(1 << (bitIndex % 8));
}

inline void clear_bits(Byte* const bytes, const int bitCnt) {
    assert(bytes);
    assert(bitCnt > 0);
    memset(bytes, 0, conv_bits_to_bytes(bitCnt));
}

inline int get_first_inactive_bit_index(const Byte* const bytes, const int bitCnt) {
    assert(bytes);
    assert(bitCnt > 0);

    for (int i = 0; i < bitCnt; ++i) {
        if (!is_bit_active(bytes, i)) {
            return i;
        }
    }

    return -1;
}

inline bool are_all_bits_active(const Byte* const bytes, const int bitCnt) {
    assert(bytes);
    assert(bitCnt > 0);

    for (int i = 0; i < bitCnt; ++i) {
        if (!is_bit_active(bytes, i)) {
            return false;
        }
    }

    return true;
}

}
