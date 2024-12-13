#include <zf3_local.h>

namespace zf3 {

static KeyCode glfw_to_key_code(const int glfwKeyCode) {
    switch (glfwKeyCode) {
        case GLFW_KEY_SPACE: return KEY_SPACE;

        case GLFW_KEY_0: return KEY_0;
        case GLFW_KEY_1: return KEY_1;
        case GLFW_KEY_2: return KEY_2;
        case GLFW_KEY_3: return KEY_3;
        case GLFW_KEY_4: return KEY_4;
        case GLFW_KEY_5: return KEY_5;
        case GLFW_KEY_6: return KEY_6;
        case GLFW_KEY_7: return KEY_7;
        case GLFW_KEY_8: return KEY_8;
        case GLFW_KEY_9: return KEY_9;

        case GLFW_KEY_A: return KEY_A;
        case GLFW_KEY_B: return KEY_B;
        case GLFW_KEY_C: return KEY_C;
        case GLFW_KEY_D: return KEY_D;
        case GLFW_KEY_E: return KEY_E;
        case GLFW_KEY_F: return KEY_F;
        case GLFW_KEY_G: return KEY_G;
        case GLFW_KEY_H: return KEY_H;
        case GLFW_KEY_I: return KEY_I;
        case GLFW_KEY_J: return KEY_J;
        case GLFW_KEY_K: return KEY_K;
        case GLFW_KEY_L: return KEY_L;
        case GLFW_KEY_M: return KEY_M;
        case GLFW_KEY_N: return KEY_N;
        case GLFW_KEY_O: return KEY_O;
        case GLFW_KEY_P: return KEY_P;
        case GLFW_KEY_Q: return KEY_Q;
        case GLFW_KEY_R: return KEY_R;
        case GLFW_KEY_S: return KEY_S;
        case GLFW_KEY_T: return KEY_T;
        case GLFW_KEY_U: return KEY_U;
        case GLFW_KEY_V: return KEY_V;
        case GLFW_KEY_W: return KEY_W;
        case GLFW_KEY_X: return KEY_X;
        case GLFW_KEY_Y: return KEY_Y;
        case GLFW_KEY_Z: return KEY_Z;

        case GLFW_KEY_ESCAPE: return KEY_ESCAPE;
        case GLFW_KEY_ENTER: return KEY_ENTER;
        case GLFW_KEY_TAB: return KEY_TAB;

        case GLFW_KEY_RIGHT: return KEY_RIGHT;
        case GLFW_KEY_LEFT: return KEY_LEFT;
        case GLFW_KEY_DOWN: return KEY_DOWN;
        case GLFW_KEY_UP: return KEY_UP;

        case GLFW_KEY_F1: return KEY_F1;
        case GLFW_KEY_F2: return KEY_F2;
        case GLFW_KEY_F3: return KEY_F3;
        case GLFW_KEY_F4: return KEY_F4;
        case GLFW_KEY_F5: return KEY_F5;
        case GLFW_KEY_F6: return KEY_F6;
        case GLFW_KEY_F7: return KEY_F7;
        case GLFW_KEY_F8: return KEY_F8;
        case GLFW_KEY_F9: return KEY_F9;
        case GLFW_KEY_F10: return KEY_F10;
        case GLFW_KEY_F11: return KEY_F11;
        case GLFW_KEY_F12: return KEY_F12;

        case GLFW_KEY_LEFT_SHIFT: return KEY_LEFT_SHIFT;
        case GLFW_KEY_LEFT_CONTROL: return KEY_LEFT_CONTROL;
        case GLFW_KEY_LEFT_ALT: return KEY_LEFT_ALT;

        default:
            assert(false);
            return INVALID_KEY_CODE;
    }
}

static MouseButtonCode glfw_to_mouse_button_code(const int buttonCode) {
    switch (buttonCode) {
        case GLFW_MOUSE_BUTTON_LEFT: return MOUSE_BUTTON_LEFT;
        case GLFW_MOUSE_BUTTON_RIGHT: return MOUSE_BUTTON_RIGHT;
        case GLFW_MOUSE_BUTTON_MIDDLE: return MOUSE_BUTTON_MID;

        default:
            assert(false);
            return INVALID_MOUSE_BUTTON_CODE;
    }
}

static void glfw_window_size_callback(GLFWwindow* const window, const int width, const int height) {
    const auto windowMeta = static_cast<WindowMeta*>(glfwGetWindowUserPointer(window));
    windowMeta->size.x = width;
    windowMeta->size.y = height;

    glViewport(0, 0, width, height);
}

static void glfw_key_callback(GLFWwindow* const window, const int key, const int scancode, const int action, const int mods) {
    const auto windowMeta = static_cast<WindowMeta*>(glfwGetWindowUserPointer(window));
    const KeysDownBitset keyBit = static_cast<KeysDownBitset>(1) << glfw_to_key_code(key);

    if (action == GLFW_PRESS) {
        windowMeta->inputState.keysDownBits |= keyBit;
    } else if (action == GLFW_RELEASE) {
        windowMeta->inputState.keysDownBits &= ~keyBit;
    }
}

static void glfw_mouse_button_callback(GLFWwindow* const window, const int button, const int action, const int mods) {
    const auto windowMeta = static_cast<WindowMeta*>(glfwGetWindowUserPointer(window));
    const MouseButtonsDownBitset buttonBit = static_cast<MouseButtonsDownBitset>(1) << glfw_to_mouse_button_code(button);

    if (action == GLFW_PRESS) {
        windowMeta->inputState.mouseButtonsDownBits |= buttonBit;
    } else if (action == GLFW_RELEASE) {
        windowMeta->inputState.mouseButtonsDownBits &= ~buttonBit;
    }
}

static void glfw_cursor_pos_callback(GLFWwindow* const window, const double x, const double y) {
    const auto windowMeta = static_cast<WindowMeta*>(glfwGetWindowUserPointer(window));
    windowMeta->inputState.mousePos.x = x;
    windowMeta->inputState.mousePos.y = y;
}

GLFWwindow* create_glfw_window(WindowMeta* const windowMeta, const int width, const int height, const char* const title, const bool resizable) {
    memset(windowMeta, 0, sizeof(*windowMeta));

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gk_glVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gk_glVersionMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, resizable);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

    GLFWwindow* const glfwWindow = glfwCreateWindow(width, height, title, NULL, NULL);

    if (glfwWindow) {
        glfwMakeContextCurrent(glfwWindow);

        glfwSetWindowUserPointer(glfwWindow, windowMeta);
        glfwSetWindowSizeCallback(glfwWindow, glfw_window_size_callback);
        glfwSetKeyCallback(glfwWindow, glfw_key_callback);
        glfwSetMouseButtonCallback(glfwWindow, glfw_mouse_button_callback);
        glfwSetCursorPosCallback(glfwWindow, glfw_cursor_pos_callback);

        glfwGetWindowSize(glfwWindow, &windowMeta->size.x, &windowMeta->size.y);
    }

    return glfwWindow;
}

}
