#include <zf3_rendering.h>

namespace zf3 {

GLID create_shader_from_src(const char* const src, const bool frag) {
    const GLID glID = glCreateShader(frag ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER);
    glShaderSource(glID, 1, &src, NULL);
    glCompileShader(glID);

    GLint compileSuccess;
    glGetShaderiv(glID, GL_COMPILE_STATUS, &compileSuccess);

    if (!compileSuccess) {
        GLchar infoLog[512];
        glGetShaderInfoLog(glID, sizeof(infoLog), NULL, infoLog);
        log_error("Failed to compile shader!\n\n%s\n", infoLog);

        glDeleteShader(glID);

        return 0;
    }

    return glID;
}

GLID create_shader_prog_from_srcs(const char* const vertShaderSrc, const char* const fragShaderSrc) {
    const GLID vertShaderGLID = create_shader_from_src(vertShaderSrc, false);

    if (!vertShaderGLID) {
        return 0;
    }

    const GLID fragShaderGLID = create_shader_from_src(fragShaderSrc, true);

    if (!fragShaderGLID) {
        glDeleteShader(vertShaderGLID);
        return 0;
    }

    const GLID progGLID = glCreateProgram();
    glAttachShader(progGLID, vertShaderGLID);
    glAttachShader(progGLID, fragShaderGLID);
    glLinkProgram(progGLID);

    // We no longer need the shaders, as they are now part of the program.
    glDeleteShader(vertShaderGLID);
    glDeleteShader(fragShaderGLID);

    return progGLID;
}

}
