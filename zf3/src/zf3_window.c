#include <zf3.h>

static int get_glfw_key_code(const ZF3KeyCode keyCode) {
    switch (keyCode) {
        case ZF3_KEY_SPACE: return GLFW_KEY_SPACE;

        case ZF3_KEY_0: return GLFW_KEY_0;
        case ZF3_KEY_1: return GLFW_KEY_1;
        case ZF3_KEY_2: return GLFW_KEY_2;
        case ZF3_KEY_3: return GLFW_KEY_3;
        case ZF3_KEY_4: return GLFW_KEY_4;
        case ZF3_KEY_5: return GLFW_KEY_5;
        case ZF3_KEY_6: return GLFW_KEY_6;
        case ZF3_KEY_7: return GLFW_KEY_7;
        case ZF3_KEY_8: return GLFW_KEY_8;
        case ZF3_KEY_9: return GLFW_KEY_9;

        case ZF3_KEY_A: return GLFW_KEY_A;
        case ZF3_KEY_B: return GLFW_KEY_B;
        case ZF3_KEY_C: return GLFW_KEY_C;
        case ZF3_KEY_D: return GLFW_KEY_D;
        case ZF3_KEY_E: return GLFW_KEY_E;
        case ZF3_KEY_F: return GLFW_KEY_F;
        case ZF3_KEY_G: return GLFW_KEY_G;
        case ZF3_KEY_H: return GLFW_KEY_H;
        case ZF3_KEY_I: return GLFW_KEY_I;
        case ZF3_KEY_J: return GLFW_KEY_J;
        case ZF3_KEY_K: return GLFW_KEY_K;
        case ZF3_KEY_L: return GLFW_KEY_L;
        case ZF3_KEY_M: return GLFW_KEY_M;
        case ZF3_KEY_N: return GLFW_KEY_N;
        case ZF3_KEY_O: return GLFW_KEY_O;
        case ZF3_KEY_P: return GLFW_KEY_P;
        case ZF3_KEY_Q: return GLFW_KEY_Q;
        case ZF3_KEY_R: return GLFW_KEY_R;
        case ZF3_KEY_S: return GLFW_KEY_S;
        case ZF3_KEY_T: return GLFW_KEY_T;
        case ZF3_KEY_U: return GLFW_KEY_U;
        case ZF3_KEY_V: return GLFW_KEY_V;
        case ZF3_KEY_W: return GLFW_KEY_W;
        case ZF3_KEY_X: return GLFW_KEY_X;
        case ZF3_KEY_Y: return GLFW_KEY_Y;
        case ZF3_KEY_Z: return GLFW_KEY_Z;

        case ZF3_KEY_ESCAPE: return GLFW_KEY_ESCAPE;
        case ZF3_KEY_ENTER: return GLFW_KEY_ENTER;
        case ZF3_KEY_TAB: return GLFW_KEY_TAB;

        case ZF3_KEY_RIGHT: return GLFW_KEY_RIGHT;
        case ZF3_KEY_LEFT: return GLFW_KEY_LEFT;
        case ZF3_KEY_DOWN: return GLFW_KEY_DOWN;
        case ZF3_KEY_UP: return GLFW_KEY_UP;

        case ZF3_KEY_F1: return GLFW_KEY_F1;
        case ZF3_KEY_F2: return GLFW_KEY_F2;
        case ZF3_KEY_F3: return GLFW_KEY_F3;
        case ZF3_KEY_F4: return GLFW_KEY_F4;
        case ZF3_KEY_F5: return GLFW_KEY_F5;
        case ZF3_KEY_F6: return GLFW_KEY_F6;
        case ZF3_KEY_F7: return GLFW_KEY_F7;
        case ZF3_KEY_F8: return GLFW_KEY_F8;
        case ZF3_KEY_F9: return GLFW_KEY_F9;
        case ZF3_KEY_F10: return GLFW_KEY_F10;
        case ZF3_KEY_F11: return GLFW_KEY_F11;
        case ZF3_KEY_F12: return GLFW_KEY_F12;

        case ZF3_KEY_LEFT_SHIFT: return GLFW_KEY_LEFT_SHIFT;
        case ZF3_KEY_LEFT_CONTROL: return GLFW_KEY_LEFT_CONTROL;
        case ZF3_KEY_LEFT_ALT: return GLFW_KEY_LEFT_ALT;

        default: return GLFW_KEY_UNKNOWN;
    }
}

static ZF3KeysDownBits get_keys_down(GLFWwindow* window) {
    ZF3KeysDownBits keysDownBits = 0;

    for (int i = 0; i < ZF3_KEY_CODE_CNT; ++i) {
        if (glfwGetKey(window, get_glfw_key_code(i)) == GLFW_PRESS) {
            keysDownBits |= (ZF3KeysDownBits)1 << i;
        }
    }

    return keysDownBits;
}

static ZF3MouseButtonsDownBits get_mouse_buttons_down(GLFWwindow* window) {
    unsigned int mouseButtonsDownBits = 0;

    for (int i = 0; i < ZF3_MOUSE_BUTTON_CODE_CNT; ++i) {
        if (glfwGetMouseButton(window, i) == GLFW_PRESS) {
            mouseButtonsDownBits |= (ZF3MouseButtonsDownBits)1 << i;
        }
    }

    return mouseButtonsDownBits;
}

static void load_gamepad_state(ZF3GamepadState* const state) {
    memset(state, 0, sizeof(*state));

    // Search for the first active gamepad and if found update the gamepad state using it.
    for (int i = 0; i <= GLFW_JOYSTICK_LAST; i++) {
        if (!glfwJoystickPresent(i) || !glfwJoystickIsGamepad(i)) {
            continue;
        }

        GLFWgamepadstate glfwGamepadState;

        if (!glfwGetGamepadState(i, &glfwGamepadState)) {
            break;
        }

        state->connected = true;
        state->glfwJoystickIndex = i;

        // Store which gamepad buttons are down.
        state->buttonsDownBits = 0;

        for (int j = 0; j < ZF3_GAMEPAD_BUTTON_CODE_CNT; j++) {
            if (glfwGamepadState.buttons[j] == GLFW_PRESS) {
                state->buttonsDownBits |= (ZF3GamepadButtonsDownBits)1 << j;
            }
        }

        // Store gamepad axis values.
        for (int j = 0; j < ZF3_GAMEPAD_AXIS_CODE_CNT; j++) {
            state->axisValues[j] = glfwGamepadState.axes[j];
        }

        break;
    }
}

GLFWwindow* zf3_create_glfw_window(const int width, const int height, const char* const title, const bool resizable) {
    assert(width > 0 && height > 0);
    assert(title && title[0]);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ZF3_GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ZF3_GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, resizable);
    glfwWindowHint(GLFW_VISIBLE, false);

    GLFWwindow* const window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (window) {
        glfwMakeContextCurrent(window);
    }

    return window;
}

void zf3_load_input_state(ZF3InputState* const inputState, GLFWwindow* const glfwWindow) {
    memset(inputState, 0, sizeof(*inputState));
    inputState->keysDownBits = get_keys_down(glfwWindow);
    inputState->mouseButtonsDownBits = get_mouse_buttons_down(glfwWindow);
    load_gamepad_state(&inputState->gamepadState);
}
