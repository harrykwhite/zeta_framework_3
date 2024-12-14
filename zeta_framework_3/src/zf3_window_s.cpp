#include <zf3_window.h>

namespace zf3 {

static GLFWwindow* i_glfwWindow;
static Vec2DInt i_windowSize;

static Vec2D i_mousePos;

using KeysDownBits = unsigned long long;
using MouseButtonsDownBits = unsigned char;

struct InputState {
    KeysDownBits keysDownBits;
    MouseButtonsDownBits mouseButtonsDownBits;
};

static InputState i_inputState;
static InputState i_inputStateLast;

static void glfw_window_size_callback(GLFWwindow* const window, const int width, const int height) {
    i_windowSize.x = width;
    i_windowSize.y = height;

    glViewport(0, 0, width, height);
}

static void glfw_key_callback(GLFWwindow* const window, const int key, const int scancode, const int action, const int mods) {
    const KeysDownBits keyBit = static_cast<KeysDownBits>(1) << glfw_to_zf3_key_code(key);

    if (action == GLFW_PRESS) {
        i_inputState.keysDownBits |= keyBit;
    } else if (action == GLFW_RELEASE) {
        i_inputState.keysDownBits &= ~keyBit;
    }
}

static void glfw_mouse_button_callback(GLFWwindow* const window, const int button, const int action, const int mods) {
    const MouseButtonsDownBits buttonBit = static_cast<MouseButtonsDownBits>(1) << glfw_to_zf3_mouse_button_code(button);

    if (action == GLFW_PRESS) {
        i_inputState.mouseButtonsDownBits |= buttonBit;
    } else if (action == GLFW_RELEASE) {
        i_inputState.mouseButtonsDownBits &= ~buttonBit;
    }
}

static void glfw_cursor_pos_callback(GLFWwindow* const window, const double x, const double y) {
    i_mousePos.x = static_cast<float>(x);
    i_mousePos.y = static_cast<float>(y);
}

bool init_window(const int width, const int height, const char* const title, const bool resizable, const bool hideCursor) {
    assert(!i_glfwWindow);
    assert(width > 0 && height > 0);
    assert(title && title[0]);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, resizable);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

    i_glfwWindow = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!i_glfwWindow) {
        return false;
    }

    glfwMakeContextCurrent(i_glfwWindow);

    glfwSetWindowSizeCallback(i_glfwWindow, glfw_window_size_callback);
    glfwSetKeyCallback(i_glfwWindow, glfw_key_callback);
    glfwSetMouseButtonCallback(i_glfwWindow, glfw_mouse_button_callback);
    glfwSetCursorPosCallback(i_glfwWindow, glfw_cursor_pos_callback);

    glfwSetInputMode(i_glfwWindow, GLFW_CURSOR, hideCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

    glfwGetWindowSize(i_glfwWindow, &i_windowSize.x, &i_windowSize.y);

    return true;
}

void clean_window() {
    if (!i_glfwWindow) {
        return;
    }

    glfwDestroyWindow(i_glfwWindow);
    i_glfwWindow = NULL;
}

void show_window() {
    assert(i_glfwWindow);
    glfwShowWindow(i_glfwWindow);
}

bool should_window_close() {
    assert(i_glfwWindow);
    return glfwWindowShouldClose(i_glfwWindow);
}

void swap_window_buffers() {
    assert(i_glfwWindow);
    glfwSwapBuffers(i_glfwWindow);
}

Vec2DInt get_window_size() {
    assert(i_glfwWindow);
    return i_windowSize;
}

void save_input_state() {
    i_inputStateLast = i_inputState;
}

bool is_key_down(const KeyCode keyCode) {
    return i_inputState.keysDownBits & (static_cast<KeysDownBits>(1) << keyCode);
}

bool is_key_pressed(const KeyCode keyCode) {
    const KeysDownBits keyBit = static_cast<KeysDownBits>(1) << keyCode;
    return (i_inputState.keysDownBits & keyBit) && !(i_inputStateLast.keysDownBits & keyBit);
}

bool is_key_released(const KeyCode keyCode) {
    const KeysDownBits keyBit = static_cast<KeysDownBits>(1) << keyCode;
    return !(i_inputState.keysDownBits & keyBit) && (i_inputStateLast.keysDownBits & keyBit);
}

bool is_mouse_button_down(const MouseButtonCode buttonCode) {
    return i_inputState.mouseButtonsDownBits & (static_cast<MouseButtonsDownBits>(1) << buttonCode);
}

bool is_mouse_button_pressed(const MouseButtonCode buttonCode) {
    const MouseButtonsDownBits buttonBit = static_cast<MouseButtonsDownBits>(1) << buttonCode;
    return (i_inputState.mouseButtonsDownBits & buttonBit) && !(i_inputStateLast.mouseButtonsDownBits & buttonBit);
}

bool is_mouse_button_released(const MouseButtonCode buttonCode) {
    const MouseButtonsDownBits buttonBit = static_cast<MouseButtonsDownBits>(1) << buttonCode;
    return !(i_inputState.mouseButtonsDownBits & buttonBit) && (i_inputStateLast.mouseButtonsDownBits & buttonBit);
}

Vec2D get_mouse_pos() {
    return i_mousePos;
}

}
