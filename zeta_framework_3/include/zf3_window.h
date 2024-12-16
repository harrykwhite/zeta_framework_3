#pragma once

#include <assert.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <zf3c.h>

namespace zf3 {
    using KeysDownBitset = unsigned long long;
    using MouseButtonsDownBitset = unsigned char;

    enum KeyCode {
        UNDEFINED_KEY_CODE = -1,

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
        UNDEFINED_MOUSE_BUTTON_CODE = -1,

        MOUSE_BUTTON_LEFT,
        MOUSE_BUTTON_RIGHT,
        MOUSE_BUTTON_MID,

        NUM_MOUSE_BUTTON_CODES
    };

    struct InputState {
        KeysDownBitset keysDown;

        MouseButtonsDownBitset mouseButtonsDown;
        Vec2D mousePos;
    };

    struct InputManager {
        InputState inputState;
        InputState inputStateSaved; // Used as a reference for press and release detection.
    };

    struct GLFWWindowCallbackData {
        Vec2DInt* windowSize;
        InputState* inputState;
    };

    struct Window {
        GLFWwindow* glfwWindow;
        GLFWWindowCallbackData glfwWindowCallbackData;
        Vec2DInt size;
    };

    bool init_window(Window* const window, InputManager* const inputManager, const int width, const int height, const char* const title, const bool resizable, const bool hideCursor);
    void clean_window(Window* const window);

    inline void save_input_state(InputManager* const inputManager) {
        inputManager->inputStateSaved = inputManager->inputState;
    }

    inline bool is_key_down(const KeyCode keyCode, const InputState* const inputState) {
        return inputState->keysDown & (static_cast<KeysDownBitset>(1) << keyCode);
    }

    inline bool is_key_down(const KeyCode keyCode, const InputManager* const inputManager) {
        return is_key_down(keyCode, &inputManager->inputState);
    }

    inline bool is_key_pressed(const KeyCode keyCode, const InputState* const inputState, const InputState* const inputStateLast) {
        return is_key_down(keyCode, inputState) && !is_key_down(keyCode, inputStateLast);
    }

    inline bool is_key_pressed(const KeyCode keyCode, const InputManager* const inputManager) {
        return is_key_pressed(keyCode, &inputManager->inputState, &inputManager->inputStateSaved);
    }

    inline bool is_key_released(const KeyCode keyCode, const InputState* const inputState, const InputState* const inputStateLast) {
        return !is_key_down(keyCode, inputState) && is_key_down(keyCode, inputStateLast);
    }

    inline bool is_key_released(const KeyCode keyCode, const InputManager* const inputManager) {
        return is_key_released(keyCode, &inputManager->inputState, &inputManager->inputStateSaved);
    }

    inline bool is_mouse_button_down(const MouseButtonCode buttonCode, const InputState* const inputState) {
        return inputState->mouseButtonsDown & (static_cast<MouseButtonsDownBitset>(1) << buttonCode);
    }

    inline bool is_mouse_button_down(const MouseButtonCode buttonCode, const InputManager* const inputManager) {
        return is_mouse_button_down(buttonCode, &inputManager->inputState);
    }

    inline bool is_mouse_button_pressed(const MouseButtonCode buttonCode, const InputState* const inputState, const InputState* const inputStateLast) {
        return is_mouse_button_down(buttonCode, inputState) && !is_mouse_button_down(buttonCode, inputStateLast);
    }

    inline bool is_mouse_button_pressed(const MouseButtonCode buttonCode, const InputManager* const inputManager) {
        return is_mouse_button_pressed(buttonCode, &inputManager->inputState, &inputManager->inputStateSaved);
    }

    inline bool is_mouse_button_released(const MouseButtonCode buttonCode, const InputState* const inputState, const InputState* const inputStateLast) {
        return !is_mouse_button_down(buttonCode, inputState) && is_mouse_button_down(buttonCode, inputStateLast);
    }

    inline bool is_mouse_button_released(const MouseButtonCode buttonCode, const InputManager* const inputManager) {
        return is_mouse_button_released(buttonCode, &inputManager->inputState, &inputManager->inputStateSaved);
    }
}
