#include <zf3_window.h>

namespace zf3 {
    static KeyCode glfw_to_zf3_key_code(const int keyCode) {
        switch (keyCode) {
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
                return UNDEFINED_KEY_CODE;
        }
    }

    static MouseButtonCode glfw_to_zf3_mouse_button_code(const int buttonCode) {
        switch (buttonCode) {
            case GLFW_MOUSE_BUTTON_LEFT: return MOUSE_BUTTON_LEFT;
            case GLFW_MOUSE_BUTTON_RIGHT: return MOUSE_BUTTON_RIGHT;
            case GLFW_MOUSE_BUTTON_MIDDLE: return MOUSE_BUTTON_MID;

            default:
                return UNDEFINED_MOUSE_BUTTON_CODE;
        }
    }

    static void glfw_window_size_callback(GLFWwindow* const window, const int width, const int height) {
        const GLFWWindowCallbackData* const cbData = static_cast<const GLFWWindowCallbackData*>(glfwGetWindowUserPointer(window));
        *cbData->windowSize = {width, height};
        glViewport(0, 0, width, height); // TODO: Move elsewhere?
    }

    static void glfw_key_callback(GLFWwindow* const window, const int key, const int scancode, const int action, const int mods) {
        const KeyCode keyCode = glfw_to_zf3_key_code(key);

        if (keyCode != UNDEFINED_KEY_CODE) {
            const GLFWWindowCallbackData* const cbData = static_cast<const GLFWWindowCallbackData*>(glfwGetWindowUserPointer(window));
            const KeysDownBitset keyBit = static_cast<KeysDownBitset>(1) << glfw_to_zf3_key_code(key);

            if (action == GLFW_PRESS) {
                cbData->inputState->keysDown |= keyBit;
            } else if (action == GLFW_RELEASE) {
                cbData->inputState->keysDown &= ~keyBit;
            }
        }
    }

    static void glfw_mouse_button_callback(GLFWwindow* const window, const int button, const int action, const int mods) {
        const MouseButtonCode buttonCode = glfw_to_zf3_mouse_button_code(button);

        if (buttonCode != UNDEFINED_MOUSE_BUTTON_CODE) {
            const GLFWWindowCallbackData* const cbData = static_cast<const GLFWWindowCallbackData*>(glfwGetWindowUserPointer(window));
            const MouseButtonsDownBitset buttonBit = static_cast<MouseButtonsDownBitset>(1) << glfw_to_zf3_mouse_button_code(button);

            if (action == GLFW_PRESS) {
                cbData->inputState->mouseButtonsDown |= buttonBit;
            } else if (action == GLFW_RELEASE) {
                cbData->inputState->mouseButtonsDown &= ~buttonBit;
            }
        }
    }

    static void glfw_cursor_pos_callback(GLFWwindow* const window, const double x, const double y) {
        const GLFWWindowCallbackData* const cbData = static_cast<const GLFWWindowCallbackData*>(glfwGetWindowUserPointer(window));
        cbData->inputState->mousePos = {static_cast<float>(x), static_cast<float>(y)};
    }

    bool init_window(Window& window, InputManager& inputManager, const int width, const int height, const char* const title, const bool resizable, const bool hideCursor) {
        assert(width > 0 && height > 0);
        assert(title && title[0]);

        zero_out(window);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, resizable);
        glfwWindowHint(GLFW_VISIBLE, false);

        window.glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);

        if (!window.glfwWindow) {
            return false;
        }

        glfwMakeContextCurrent(window.glfwWindow);

        window.glfwWindowCallbackData = {
            .windowSize = &window.size,
            .inputState = &inputManager.inputState
        };

        glfwSetWindowUserPointer(window.glfwWindow, &window.glfwWindowCallbackData);

        glfwSetWindowSizeCallback(window.glfwWindow, glfw_window_size_callback);
        glfwSetKeyCallback(window.glfwWindow, glfw_key_callback);
        glfwSetMouseButtonCallback(window.glfwWindow, glfw_mouse_button_callback);
        glfwSetCursorPosCallback(window.glfwWindow, glfw_cursor_pos_callback);

        glfwSetInputMode(window.glfwWindow, GLFW_CURSOR, hideCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

        glfwGetWindowSize(window.glfwWindow, &window.size.x, &window.size.y);

        return true;
    }

    void clean_window(Window& window) {
        if (window.glfwWindow) {
            glfwDestroyWindow(window.glfwWindow);
        }

        zero_out(window);
    }
}
