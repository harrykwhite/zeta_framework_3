#include <zf3_local.h>

static ZF3KeyCode glfw_to_zf3_key_code(const int glfwKeyCode) {
    switch (glfwKeyCode) {
        case GLFW_KEY_SPACE: return ZF3_KEY_SPACE;

        case GLFW_KEY_0: return ZF3_KEY_0;
        case GLFW_KEY_1: return ZF3_KEY_1;
        case GLFW_KEY_2: return ZF3_KEY_2;
        case GLFW_KEY_3: return ZF3_KEY_3;
        case GLFW_KEY_4: return ZF3_KEY_4;
        case GLFW_KEY_5: return ZF3_KEY_5;
        case GLFW_KEY_6: return ZF3_KEY_6;
        case GLFW_KEY_7: return ZF3_KEY_7;
        case GLFW_KEY_8: return ZF3_KEY_8;
        case GLFW_KEY_9: return ZF3_KEY_9;

        case GLFW_KEY_A: return ZF3_KEY_A;
        case GLFW_KEY_B: return ZF3_KEY_B;
        case GLFW_KEY_C: return ZF3_KEY_C;
        case GLFW_KEY_D: return ZF3_KEY_D;
        case GLFW_KEY_E: return ZF3_KEY_E;
        case GLFW_KEY_F: return ZF3_KEY_F;
        case GLFW_KEY_G: return ZF3_KEY_G;
        case GLFW_KEY_H: return ZF3_KEY_H;
        case GLFW_KEY_I: return ZF3_KEY_I;
        case GLFW_KEY_J: return ZF3_KEY_J;
        case GLFW_KEY_K: return ZF3_KEY_K;
        case GLFW_KEY_L: return ZF3_KEY_L;
        case GLFW_KEY_M: return ZF3_KEY_M;
        case GLFW_KEY_N: return ZF3_KEY_N;
        case GLFW_KEY_O: return ZF3_KEY_O;
        case GLFW_KEY_P: return ZF3_KEY_P;
        case GLFW_KEY_Q: return ZF3_KEY_Q;
        case GLFW_KEY_R: return ZF3_KEY_R;
        case GLFW_KEY_S: return ZF3_KEY_S;
        case GLFW_KEY_T: return ZF3_KEY_T;
        case GLFW_KEY_U: return ZF3_KEY_U;
        case GLFW_KEY_V: return ZF3_KEY_V;
        case GLFW_KEY_W: return ZF3_KEY_W;
        case GLFW_KEY_X: return ZF3_KEY_X;
        case GLFW_KEY_Y: return ZF3_KEY_Y;
        case GLFW_KEY_Z: return ZF3_KEY_Z;

        case GLFW_KEY_ESCAPE: return ZF3_KEY_ESCAPE;
        case GLFW_KEY_ENTER: return ZF3_KEY_ENTER;
        case GLFW_KEY_TAB: return ZF3_KEY_TAB;

        case GLFW_KEY_RIGHT: return ZF3_KEY_RIGHT;
        case GLFW_KEY_LEFT: return ZF3_KEY_LEFT;
        case GLFW_KEY_DOWN: return ZF3_KEY_DOWN;
        case GLFW_KEY_UP: return ZF3_KEY_UP;

        case GLFW_KEY_F1: return ZF3_KEY_F1;
        case GLFW_KEY_F2: return ZF3_KEY_F2;
        case GLFW_KEY_F3: return ZF3_KEY_F3;
        case GLFW_KEY_F4: return ZF3_KEY_F4;
        case GLFW_KEY_F5: return ZF3_KEY_F5;
        case GLFW_KEY_F6: return ZF3_KEY_F6;
        case GLFW_KEY_F7: return ZF3_KEY_F7;
        case GLFW_KEY_F8: return ZF3_KEY_F8;
        case GLFW_KEY_F9: return ZF3_KEY_F9;
        case GLFW_KEY_F10: return ZF3_KEY_F10;
        case GLFW_KEY_F11: return ZF3_KEY_F11;
        case GLFW_KEY_F12: return ZF3_KEY_F12;

        case GLFW_KEY_LEFT_SHIFT: return ZF3_KEY_LEFT_SHIFT;
        case GLFW_KEY_LEFT_CONTROL: return ZF3_KEY_LEFT_CONTROL;
        case GLFW_KEY_LEFT_ALT: return ZF3_KEY_LEFT_ALT;

        default:
            assert(false);
            return -1;
    }
}

static ZF3MouseButtonCode glfw_to_zf3_mouse_button_code(const int buttonCode) {
    switch (buttonCode) {
        case GLFW_MOUSE_BUTTON_LEFT: return ZF3_MOUSE_BUTTON_LEFT;
        case GLFW_MOUSE_BUTTON_RIGHT: return ZF3_MOUSE_BUTTON_RIGHT;
        case GLFW_MOUSE_BUTTON_MIDDLE: return ZF3_MOUSE_BUTTON_MID;

        default:
            assert(false);
            return -1;
    }
}

static void glfw_window_size_callback(GLFWwindow* const window, const int width, const int height) {
    ZF3WindowMeta* const windowMeta = glfwGetWindowUserPointer(window);
    windowMeta->size.x = width;
    windowMeta->size.y = height;

    glViewport(0, 0, width, height);
}

static void glfw_key_callback(GLFWwindow* const window, const int key, const int scancode, const int action, const int mods) {
    ZF3WindowMeta* const windowMeta = glfwGetWindowUserPointer(window);
    const ZF3KeysDownBitset keyBit = (ZF3KeysDownBitset)1 << glfw_to_zf3_key_code(key);

    if (action == GLFW_PRESS) {
        windowMeta->inputState.keysDownBits |= keyBit;
    } else if (action == GLFW_RELEASE) {
        windowMeta->inputState.keysDownBits &= ~keyBit;
    }
}

static void glfw_mouse_button_callback(GLFWwindow* const window, const int button, const int action, const int mods) {
    ZF3WindowMeta* const windowMeta = glfwGetWindowUserPointer(window);
    const ZF3MouseButtonsDownBitset buttonBit = (ZF3MouseButtonsDownBitset)1 << glfw_to_zf3_mouse_button_code(button);

    if (action == GLFW_PRESS) {
        windowMeta->inputState.mouseButtonsDownBits |= buttonBit;
    } else if (action == GLFW_RELEASE) {
        windowMeta->inputState.mouseButtonsDownBits &= ~buttonBit;
    }
}

static void glfw_cursor_pos_callback(GLFWwindow* const window, const double x, const double y) {
    ZF3WindowMeta* const windowMeta = glfwGetWindowUserPointer(window);
    windowMeta->inputState.mousePos.x = x;
    windowMeta->inputState.mousePos.y = y;
}

GLFWwindow* zf3_create_glfw_window(ZF3WindowMeta* const windowMeta, const int width, const int height, const char* const title, const bool resizable) {
    memset(windowMeta, 0, sizeof(*windowMeta));

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ZF3_GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ZF3_GL_VERSION_MINOR);
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
