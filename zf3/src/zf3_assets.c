#include <zf3.h>

static const char* const s_spriteVertShaderSrc =
"#version 430 core\n"
"\n"
"layout (location = 0) in vec2 a_vert;\n"
"\n"
"out vec2 v_texCoord;\n"
"\n"
"uniform mat4 u_view;\n"
"uniform mat4 u_proj;\n"
"uniform vec2 u_pos;\n"
"uniform vec2 u_size;\n"
"uniform float u_rot;\n"
"\n"
"void main()\n"
"{\n"
"    float rotCos = cos(u_rot);\n"
"    float rotSin = -sin(u_rot);\n"
"\n"
"    mat4 model = mat4(\n"
"        vec4(u_size.x * rotCos, u_size.x * rotSin, 0.0f, 0.0f),\n"
"        vec4(u_size.y * -rotSin, u_size.y * rotCos, 0.0f, 0.0f),\n"
"        vec4(0.0f, 0.0f, 1.0f, 0.0f),\n"
"        vec4(u_pos.x, u_pos.y, 0.0f, 1.0f)\n"
"    );\n"
"\n"
"    gl_Position = u_proj * u_view * model * vec4(a_vert, 0.0f, 1.0f);\n"
"\n"
"    v_texCoord = a_vert;\n"
"}\n";

const char* const s_spriteFragShaderSrc =
"#version 430 core\n"
"\n"
"in vec2 v_texCoord;\n"
"\n"
"out vec4 o_fragColor;\n"
"\n"
"uniform float u_alpha;\n"
"uniform sampler2D u_tex;\n"
"\n"
"void main()\n"
"{\n"
"    vec4 texColor = texture(u_tex, v_texCoord);\n"
"    o_fragColor = texColor * vec4(1.0f, 1.0f, 1.0f, u_alpha);\n"
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
        printf("ZF2 ERROR: Failed to compile shader!\n\n%s\n\n", infoLog);

        glDeleteShader(glID);

        return 0;
    }

    return glID;
}

static GLuint create_shader_prog_from_srcs(const char* const vertShaderSrc, const char* const fragShaderSrc) {
    // Create the shaders.
    const GLuint vertShaderGLID = create_shader_from_src(vertShaderSrc, false);

    if (!vertShaderGLID) {
        return 0;
    }

    const GLuint fragShaderGLID = create_shader_from_src(fragShaderSrc, true);

    if (!fragShaderGLID) {
        glDeleteShader(vertShaderGLID);
        return 0;
    }

    // Create the shader program.
    const GLuint progGLID = glCreateProgram();
    glAttachShader(progGLID, vertShaderGLID);
    glAttachShader(progGLID, fragShaderGLID);
    glLinkProgram(progGLID);

    // Delete the shaders as they are no longer needed.
    glDeleteShader(vertShaderGLID);
    glDeleteShader(fragShaderGLID);

    return progGLID;
}

bool zf3_load_assets(ZF3Assets* const assets, ZF3MemArena* const scratchSpace) {
    assert(zf3_is_zero(assets, sizeof(*assets)));

    // Open the assets file.
    FILE* const fs = fopen(ZF3_ASSETS_FILE_NAME, "rb");

    if (!fs) {
        return false;
    }

    // Read asset counts.
    fread(&assets->texCnt, sizeof(assets->texCnt), 1, fs);

    //
    // Textures
    //
    if (assets->texCnt > 0) {
        // Generate textures and store their IDs.
        glGenTextures(assets->texCnt, assets->texGLIDs);

        // Read the sizes and pixel data of textures and finish setting them up.
        const int pxDataBufSize = ZF3_TEX_CHANNEL_COUNT * ZF3_TEX_WIDTH_LIMIT * ZF3_TEX_HEIGHT_LIMIT;
        unsigned char* const pxDataBuf = zf3_mem_arena_push(scratchSpace, pxDataBufSize); // A buffer for temporarily storing the pixel data of each texture.
        assert(pxDataBuf);

        for (int i = 0; i < assets->texCnt; ++i) {
            fread(&assets->texSizes[i], sizeof(assets->texSizes[i]), 1, fs);

            fread(pxDataBuf, 1, ZF3_TEX_CHANNEL_COUNT * assets->texSizes[i].x * assets->texSizes[i].y, fs);

            glBindTexture(GL_TEXTURE_2D, assets->texGLIDs[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, assets->texSizes[i].x, assets->texSizes[i].y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pxDataBuf);
        }
    }

    fclose(fs);

    return true;
}

void zf3_unload_assets(ZF3Assets* const assets) {
    glDeleteTextures(assets->texCnt, assets->texGLIDs);
    memset(assets, 0, sizeof(*assets));
}

void zf3_load_shader_progs(ZF3ShaderProgs* const shaderProgs) {
    assert(zf3_is_zero(shaderProgs, sizeof(*shaderProgs)));

    shaderProgs->spriteGLID = create_shader_prog_from_srcs(s_spriteVertShaderSrc, s_spriteFragShaderSrc);
    assert(shaderProgs->spriteGLID);

    shaderProgs->spriteViewUniLoc = glGetUniformLocation(shaderProgs->spriteGLID, "u_view");
    shaderProgs->spriteProjUniLoc = glGetUniformLocation(shaderProgs->spriteGLID, "u_proj");
    shaderProgs->spritePosUniLoc = glGetUniformLocation(shaderProgs->spriteGLID, "u_pos");
    shaderProgs->spriteSizeUniLoc = glGetUniformLocation(shaderProgs->spriteGLID, "u_size");
    shaderProgs->spriteRotUniLoc = glGetUniformLocation(shaderProgs->spriteGLID, "u_rot");
    shaderProgs->spriteAlphaUniLoc = glGetUniformLocation(shaderProgs->spriteGLID, "u_alpha");
    shaderProgs->spriteTexUniLoc = glGetUniformLocation(shaderProgs->spriteGLID, "u_tex");
}

void zf3_unload_shader_progs(ZF3ShaderProgs* const shaderProgs) {
    glDeleteProgram(shaderProgs->spriteGLID);
    memset(shaderProgs, 0, sizeof(*shaderProgs));
}
