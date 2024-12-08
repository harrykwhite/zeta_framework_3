#include <zf3.h>

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
