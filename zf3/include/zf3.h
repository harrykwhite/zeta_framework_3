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

#define ZF3_KILOBYTES(X) ((X) << 10)
#define ZF3_MEGABYTES(X) ((X) << 20)
#define ZF3_GIGABYTES(X) ((X) << 30)

#define ZF3_GL_VERSION_MAJOR 4
#define ZF3_GL_VERSION_MINOR 1

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
typedef unsigned long long ZF3KeysDownBits;
typedef unsigned char ZF3MouseButtonsDownBits;
typedef unsigned short ZF3GamepadButtonsDownBits;

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

typedef enum {
    ZF3_GAMEPAD_BUTTON_A,
    ZF3_GAMEPAD_BUTTON_B,
    ZF3_GAMEPAD_BUTTON_X,
    ZF3_GAMEPAD_BUTTON_Y,

    ZF3_GAMEPAD_BUTTON_LEFT_BUMPER,
    ZF3_GAMEPAD_BUTTON_RIGHT_BUMPER,

    ZF3_GAMEPAD_BUTTON_BACK,
    ZF3_GAMEPAD_BUTTON_START,
    ZF3_GAMEPAD_BUTTON_GUIDE,

    ZF3_GAMEPAD_BUTTON_LEFT_THUMB,
    ZF3_GAMEPAD_BUTTON_RIGHT_THUMB,

    ZF3_GAMEPAD_BUTTON_DPAD_UP,
    ZF3_GAMEPAD_BUTTON_DPAD_RIGHT,
    ZF3_GAMEPAD_BUTTON_DPAD_DOWN,
    ZF3_GAMEPAD_BUTTON_DPAD_LEFT,

    ZF3_GAMEPAD_BUTTON_CODE_CNT
} ZF3GamepadButtonCode;

typedef enum {
    ZF3_GAMEPAD_AXIS_LEFT_X,
    ZF3_GAMEPAD_AXIS_LEFT_Y,

    ZF3_GAMEPAD_AXIS_RIGHT_X,
    ZF3_GAMEPAD_AXIS_RIGHT_Y,

    ZF3_GAMEPAD_AXIS_LEFT_TRIGGER,
    ZF3_GAMEPAD_AXIS_RIGHT_TRIGGER,

    ZF3_GAMEPAD_AXIS_CODE_CNT
} ZF3GamepadAxisCode;

typedef struct {
    bool connected; // Included to support zero-initialisation.
    int glfwJoystickIndex;
    ZF3GamepadButtonsDownBits buttonsDownBits;
    float axisValues[ZF3_GAMEPAD_AXIS_CODE_CNT];
} ZF3GamepadState;

typedef struct {
    ZF3KeysDownBits keysDownBits;
    ZF3MouseButtonsDownBits mouseButtonsDownBits;
    ZF3GamepadState gamepadState;
} ZF3InputState;

GLFWwindow* zf3_create_glfw_window(const int width, const int height, const char* const title, const bool resizable);

void zf3_load_input_state(ZF3InputState* const inputState, GLFWwindow* const glfwWindow);

inline bool zf3_is_key_down(const ZF3KeyCode keyCode, const ZF3InputState* const inputState) {
    return inputState->keysDownBits & ((ZF3KeysDownBits)1 << keyCode);
}

inline bool zf3_is_key_pressed(const ZF3KeyCode keyCode, const ZF3InputState* const inputState, const ZF3InputState* const inputStateLast) {
    const ZF3KeysDownBits keyBit = (ZF3KeysDownBits)1 << keyCode;
    return (inputState->keysDownBits & keyBit) && !(inputStateLast->keysDownBits & keyBit);
}

inline bool zf3_is_key_released(const ZF3KeyCode keyCode, const ZF3InputState* const inputState, const ZF3InputState* const inputStateLast) {
    const ZF3KeysDownBits keyBit = (ZF3KeysDownBits)1 << keyCode;
    return !(inputState->keysDownBits & keyBit) && (inputStateLast->keysDownBits & keyBit);
}

inline bool zf3_is_mouse_button_down(const ZF3MouseButtonCode mouseButtonCode, const ZF3InputState* const inputState) {
    return inputState->mouseButtonsDownBits & ((ZF3MouseButtonsDownBits)1 << mouseButtonCode);
}

inline bool zf3_is_mouse_button_pressed(const ZF3MouseButtonCode mouseButtonCode, const ZF3InputState* const inputState, const ZF3InputState* const inputStateLast) {
    const ZF3MouseButtonsDownBits mouseButtonBit = (ZF3MouseButtonsDownBits)1 << mouseButtonCode;
    return (inputState->mouseButtonsDownBits & mouseButtonBit) && !(inputStateLast->mouseButtonsDownBits & mouseButtonBit);
}

inline bool zf3_is_mouse_button_released(const ZF3MouseButtonCode mouseButtonCode, const ZF3InputState* const inputState, const ZF3InputState* const inputStateLast) {
    const ZF3MouseButtonsDownBits mouseButtonBit = (ZF3MouseButtonsDownBits)1 << mouseButtonCode;
    return !(inputState->mouseButtonsDownBits & mouseButtonBit) && (inputStateLast->mouseButtonsDownBits & mouseButtonBit);
}

inline bool zf3_is_gamepad_button_down(const ZF3GamepadButtonCode gamepadButtonCode, const ZF3InputState* const inputState) {
    return inputState->gamepadState.buttonsDownBits & ((ZF3GamepadButtonsDownBits)1 << gamepadButtonCode);
}

inline bool zf3_is_gamepad_button_pressed(const ZF3GamepadButtonCode gamepadButtonCode, const ZF3InputState* const inputState, const ZF3InputState* const inputStateLast) {
    const ZF3GamepadButtonsDownBits gamepadButtonBit = (ZF3GamepadButtonsDownBits)1 << gamepadButtonCode;
    return (inputState->gamepadState.buttonsDownBits & gamepadButtonBit) && !(inputStateLast->gamepadState.buttonsDownBits & gamepadButtonBit);
}

inline bool zf3_is_gamepad_button_released(const ZF3GamepadButtonCode gamepadButtonCode, const ZF3InputState* const inputState, const ZF3InputState* const inputStateLast) {
    const ZF3GamepadButtonsDownBits gamepadButtonBit = (ZF3GamepadButtonsDownBits)1 << gamepadButtonCode;
    return !(inputState->gamepadState.buttonsDownBits & gamepadButtonBit) && (inputStateLast->gamepadState.buttonsDownBits & gamepadButtonBit);
}

//
// Assets
//
typedef struct {
    GLuint spriteQuadGLID;
    int spriteQuadProjUniLoc;
    int spriteQuadViewUniLoc;
    int spriteQuadTexturesUniLoc;

    GLuint charQuadGLID;
    int charQuadProjUniLoc;
    int charQuadViewUniLoc;
    int charQuadPosUniLoc;
    int charQuadRotUniLoc;
    int charQuadBlendUniLoc;
} ZF3ShaderProgs;

void zf3_load_shader_progs(ZF3ShaderProgs* const shaderProgs);
void zf3_unload_shader_progs(ZF3ShaderProgs* const shaderProgs);

//
// Utilities
//
typedef unsigned char ZF3Byte;

typedef struct {
    ZF3Byte* buf;
    int size;
    int used;
} ZF3MemArena;

bool zf3_mem_arena_init(ZF3MemArena* const memArena, const int size);
void* zf3_mem_arena_push(ZF3MemArena* const memArena, const int size);
void zf3_mem_arena_reset(ZF3MemArena* const memArena);
void zf3_mem_arena_cleanup(ZF3MemArena* const memArena);

#endif
