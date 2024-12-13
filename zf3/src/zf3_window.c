#include <zf3.h>

typedef unsigned long long KeysDownBits;
typedef unsigned char MouseButtonsDownBits;

typedef struct {
    KeysDownBits keysDownBits;
    MouseButtonsDownBits mouseButtonsDownBits;
} InputState;

GLFWwindow* i_glfwWindow;

InputState i_inputState;
InputState i_inputStateLast;

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

static void glfw_key_callback(GLFWwindow* const window, const int key, const int scancode, const int action, const int mods) {
    const KeysDownBits keyBit = (KeysDownBits)1 << glfw_to_zf3_key_code(key);

    if (action == GLFW_PRESS) {
        i_inputState.keysDownBits |= keyBit;
    } else if (action == GLFW_RELEASE) {
        i_inputState.keysDownBits &= ~keyBit;
    }
}

static void glfw_mouse_button_callback(GLFWwindow* const window, const int button, const int action, const int mods) {
    const MouseButtonsDownBits buttonBit = (MouseButtonsDownBits)1 << glfw_to_zf3_mouse_button_code(button);

    if (action == GLFW_PRESS) {
        i_inputState.mouseButtonsDownBits |= buttonBit;
    } else if (action == GLFW_RELEASE) {
        i_inputState.mouseButtonsDownBits &= ~buttonBit;
    }
}

bool zf3_window_init(const int width, const int height, const char* const title, const bool resizable) {
    assert(!i_glfwWindow);
    assert(width > 0 && height > 0);
    assert(title && title[0]);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ZF3_GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ZF3_GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, resizable);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

    i_glfwWindow = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!i_glfwWindow) {
        return false;
    }

    glfwMakeContextCurrent(i_glfwWindow);

    glfwSetKeyCallback(i_glfwWindow, glfw_key_callback);
    glfwSetMouseButtonCallback(i_glfwWindow, glfw_mouse_button_callback);

    return true;
}

void zf3_window_cleanup() {
    assert(i_glfwWindow);
    glfwDestroyWindow(i_glfwWindow);
    i_glfwWindow = NULL;
}

void zf3_show_window() {
    assert(i_glfwWindow);
    glfwShowWindow(i_glfwWindow);
}

bool zf3_should_window_close() {
    assert(i_glfwWindow);
    return glfwWindowShouldClose(i_glfwWindow);
}

void zf3_swap_buffers() {
    assert(i_glfwWindow);
    glfwSwapBuffers(i_glfwWindow);
}

ZF3Vec2DInt zf3_get_window_size() {
    assert(i_glfwWindow);

    ZF3Vec2DInt size;
    glfwGetWindowSize(i_glfwWindow, &size.x, &size.y);
    return size;
}

void zf3_save_input_state() {
    i_inputStateLast = i_inputState;
}

bool zf3_is_key_down(const ZF3KeyCode keyCode) {
    return i_inputState.keysDownBits & ((KeysDownBits)1 << keyCode);
}

bool zf3_is_key_pressed(const ZF3KeyCode keyCode) {
    const KeysDownBits keyBit = (KeysDownBits)1 << keyCode;
    return (i_inputState.keysDownBits & keyBit) && !(i_inputStateLast.keysDownBits & keyBit);
}

bool zf3_is_key_released(const ZF3KeyCode keyCode) {
    const KeysDownBits keyBit = (KeysDownBits)1 << keyCode;
    return !(i_inputState.keysDownBits & keyBit) && (i_inputStateLast.keysDownBits & keyBit);
}

bool zf3_is_mouse_button_down(const ZF3MouseButtonCode buttonCode) {
    return i_inputState.mouseButtonsDownBits & ((MouseButtonsDownBits)1 << buttonCode);
}

bool zf3_is_mouse_button_pressed(const ZF3MouseButtonCode buttonCode) {
    const MouseButtonsDownBits buttonBit = (MouseButtonsDownBits)1 << buttonCode;
    return (i_inputState.mouseButtonsDownBits & buttonBit) && !(i_inputStateLast.mouseButtonsDownBits & buttonBit);
}

bool zf3_is_mouse_button_released(const ZF3MouseButtonCode buttonCode) {
    const MouseButtonsDownBits buttonBit = (MouseButtonsDownBits)1 << buttonCode;
    return !(i_inputState.mouseButtonsDownBits & buttonBit) && (i_inputStateLast.mouseButtonsDownBits & buttonBit);
}
