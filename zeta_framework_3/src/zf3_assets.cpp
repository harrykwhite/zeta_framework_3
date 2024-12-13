#include <zf3_local.h>

namespace zf3 {

static const char* i_spriteQuadVertShaderSrc =
"#version 430 core\n"
"layout (location = 0) in vec2 a_vert;\n"
"layout (location = 1) in vec2 a_pos;\n"
"layout (location = 2) in vec2 a_size;\n"
"layout (location = 3) in float a_rot;\n"
"layout (location = 4) in float a_texIndex;\n"
"layout (location = 5) in vec2 a_texCoord;\n"
"layout (location = 6) in float a_alpha;\n"
"\n"
"out flat int v_texIndex;\n"
"out vec2 v_texCoord;\n"
"out float v_alpha;\n"
"\n"
"uniform mat4 u_view;\n"
"uniform mat4 u_proj;\n"
"\n"
"void main()\n"
"{\n"
"    float rotCos = cos(a_rot);\n"
"    float rotSin = -sin(a_rot);\n"
"\n"
"    mat4 model = mat4(\n"
"        vec4(a_size.x * rotCos, a_size.x * rotSin, 0.0f, 0.0f),\n"
"        vec4(a_size.y * -rotSin, a_size.y * rotCos, 0.0f, 0.0f),\n"
"        vec4(0.0f, 0.0f, 1.0f, 0.0f),\n"
"        vec4(a_pos.x, a_pos.y, 0.0f, 1.0f)\n"
"    );\n"
"\n"
"    gl_Position = u_proj * u_view * model * vec4(a_vert, 0.0f, 1.0f);\n"
"\n"
"    v_texIndex = int(a_texIndex);\n"
"    v_texCoord = a_texCoord;\n"
"    v_alpha = a_alpha;\n"
"}\n";

static const char* i_spriteQuadFragShaderSrc =
"#version 430 core\n"
"\n"
"in flat int v_texIndex;\n"
"in vec2 v_texCoord;\n"
"in float v_alpha;\n"
"\n"
"out vec4 o_fragColor;\n"
"\n"
"uniform sampler2D u_textures[32];\n"
"\n"
"void main()\n"
"{\n"
"    vec4 texColor = texture(u_textures[v_texIndex], v_texCoord);\n"
"    o_fragColor = texColor * vec4(1.0f, 1.0f, 1.0f, v_alpha);\n"
"}\n";

static GLuint create_shader_from_src(const char* const src, const bool frag) {
    const GLuint glID = glCreateShader(frag ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER);
    glShaderSource(glID, 1, &src, NULL);
    glCompileShader(glID);

    GLint compileSuccess;
    glGetShaderiv(glID, GL_COMPILE_STATUS, &compileSuccess);

    if (!compileSuccess) {
        GLchar infoLog[512];
        glGetShaderInfoLog(glID, sizeof(infoLog), NULL, infoLog);
        printf(" ERROR: Failed to compile shader!\n\n%s\n\n", infoLog);

        glDeleteShader(glID);

        return 0;
    }

    return glID;
}

static GLuint create_shader_prog_from_srcs(const char* const vertShaderSrc, const char* const fragShaderSrc) {
    const GLuint vertShaderGLID = create_shader_from_src(vertShaderSrc, false);

    if (!vertShaderGLID) {
        return 0;
    }

    const GLuint fragShaderGLID = create_shader_from_src(fragShaderSrc, true);

    if (!fragShaderGLID) {
        glDeleteShader(vertShaderGLID);
        return 0;
    }

    const GLuint progGLID = glCreateProgram();
    glAttachShader(progGLID, vertShaderGLID);
    glAttachShader(progGLID, fragShaderGLID);
    glLinkProgram(progGLID);

    // We no longer need the shaders, as they are now part of the program.
    glDeleteShader(vertShaderGLID);
    glDeleteShader(fragShaderGLID);

    return progGLID;
}

static bool load_textures(Assets* const assets, FILE* const fs) {
    if (assets->texCnt > 0) {
        const int pxDataBufSize = gk_texChannelCnt * TEX_WIDTH_LIMIT * TEX_HEIGHT_LIMIT;
        unsigned char* const pxDataBuf = static_cast<unsigned char*>(malloc(pxDataBufSize)); // For temporarily storing the pixel data of each texture.

        if (!pxDataBuf) {
            return false;
        }

        glGenTextures(assets->texCnt, assets->texGLIDs);

        // Read the sizes and pixel data of textures and finish setting them up.
        for (int i = 0; i < assets->texCnt; ++i) {
            fread(&assets->texSizes[i], sizeof(assets->texSizes[i]), 1, fs);

            fread(pxDataBuf, gk_texChannelCnt * assets->texSizes[i].x * assets->texSizes[i].y, 1, fs);

            glBindTexture(GL_TEXTURE_2D, assets->texGLIDs[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, assets->texSizes[i].x, assets->texSizes[i].y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pxDataBuf);
        }

        free(pxDataBuf);
    }

    return true;
}

bool load_assets(Assets* const assets) {
    memset(assets, 0, sizeof(*assets));

    FILE* const fs = fopen(gk_assetsFileName, "rb");

    if (!fs) {
        return false;
    }

    fread(&assets->texCnt, sizeof(assets->texCnt), 1, fs);

    const bool loadSucceeded = load_textures(assets, fs);

    fclose(fs);

    return loadSucceeded;
}

void unload_assets(Assets* const assets) {
    glDeleteTextures(assets->texCnt, assets->texGLIDs);
    memset(assets, 0, sizeof(*assets));
}

void load_shader_progs(ShaderProgs* const progs) {
    memset(progs, 0, sizeof(*progs));

    progs->spriteQuadGLID = create_shader_prog_from_srcs(i_spriteQuadVertShaderSrc, i_spriteQuadFragShaderSrc);
    assert(progs->spriteQuadGLID);

    progs->spriteQuadProjUniLoc = glGetUniformLocation(progs->spriteQuadGLID, "u_proj");
    progs->spriteQuadViewUniLoc = glGetUniformLocation(progs->spriteQuadGLID, "u_view");
    progs->spriteQuadTexturesUniLoc = glGetUniformLocation(progs->spriteQuadGLID, "u_textures");
}

void unload_shader_progs(ShaderProgs* const progs) {
    glDeleteProgram(progs->spriteQuadGLID);
    memset(progs, 0, sizeof(*progs));
}

}
