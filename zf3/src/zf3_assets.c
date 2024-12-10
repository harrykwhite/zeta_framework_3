#include <zf3.h>

static ZF3Assets* i_assets;
static ZF3ShaderProgs* i_shaderProgs;

static const char* const i_spriteVertShaderSrc =
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

const char* const i_spriteFragShaderSrc =
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

static bool load_textures(FILE* const fs) {
    assert(i_assets);
    assert(i_assets->texCnt > 0);

    // Generate textures and store their IDs.
    glGenTextures(i_assets->texCnt, i_assets->texGLIDs);

    // Read the sizes and pixel data of textures and finish setting them up.
    const int pxDataBufSize = ZF3_TEX_CHANNEL_COUNT * ZF3_TEX_WIDTH_LIMIT * ZF3_TEX_HEIGHT_LIMIT;
    unsigned char* const pxDataBuf = malloc(pxDataBufSize); // A buffer for temporarily storing the pixel data of each texture.

    if (!pxDataBuf) {
        return false;
    }

    bool err = false;

    for (int i = 0; i < i_assets->texCnt; ++i) {
        if (fread(&i_assets->texSizes[i], sizeof(i_assets->texSizes[i]), 1, fs) < 1) {
            err = true;
            break;
        }

        if (fread(pxDataBuf, ZF3_TEX_CHANNEL_COUNT * i_assets->texSizes[i].x * i_assets->texSizes[i].y, 1, fs) < 1) {
            err = true;
            break;
        }

        glBindTexture(GL_TEXTURE_2D, i_assets->texGLIDs[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i_assets->texSizes[i].x, i_assets->texSizes[i].y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pxDataBuf);
    }

    free(pxDataBuf);

    if (err) {
        glDeleteTextures(i_assets->texCnt, i_assets->texGLIDs);
        //memset(i_assets->texGLIDs, 0, sizeof(i_assets->texGLIDs));
        //memset(i_assets->texSizes, 0, sizeof(i_assets->texSizes));
        return false;
    }

    return true;
}

bool zf3_load_assets() {
    assert(!i_assets);

    // Allocate the assets.
    i_assets = calloc(1, sizeof(*i_assets));

    if (!i_assets) {
        return false;
    }

    // Open the assets file.
    FILE* const fs = fopen(ZF3_ASSETS_FILE_NAME, "rb");

    if (!fs) {
        free(i_assets);
        i_assets = NULL;
        return false;
    }

    // Read asset counts.
    fread(&i_assets->texCnt, sizeof(i_assets->texCnt), 1, fs);

    // Load assets.
    const bool loadErr = !load_textures(fs);

    fclose(fs);

    if (loadErr) {
        free(i_assets);
        i_assets = NULL;
        return false;
    }

    return true;
}

void zf3_unload_assets() {
    assert(i_assets);
    glDeleteTextures(i_assets->texCnt, i_assets->texGLIDs);
    free(i_assets);
    i_assets = NULL;
}

const ZF3Assets* zf3_get_assets() {
    assert(i_assets);
    return i_assets;
}

bool zf3_load_shader_progs() {
    assert(!i_shaderProgs);

    i_shaderProgs = calloc(1, sizeof(*i_shaderProgs));

    if (!i_shaderProgs) {
        return false;
    }

    i_shaderProgs->spriteGLID = create_shader_prog_from_srcs(i_spriteVertShaderSrc, i_spriteFragShaderSrc);
    assert(i_shaderProgs->spriteGLID);

    i_shaderProgs->spriteViewUniLoc = glGetUniformLocation(i_shaderProgs->spriteGLID, "u_view");
    i_shaderProgs->spriteProjUniLoc = glGetUniformLocation(i_shaderProgs->spriteGLID, "u_proj");
    i_shaderProgs->spritePosUniLoc = glGetUniformLocation(i_shaderProgs->spriteGLID, "u_pos");
    i_shaderProgs->spriteSizeUniLoc = glGetUniformLocation(i_shaderProgs->spriteGLID, "u_size");
    i_shaderProgs->spriteRotUniLoc = glGetUniformLocation(i_shaderProgs->spriteGLID, "u_rot");
    i_shaderProgs->spriteAlphaUniLoc = glGetUniformLocation(i_shaderProgs->spriteGLID, "u_alpha");
    i_shaderProgs->spriteTexUniLoc = glGetUniformLocation(i_shaderProgs->spriteGLID, "u_tex");

    return true;
}

void zf3_unload_shader_progs() {
    assert(i_shaderProgs);
    glDeleteProgram(i_shaderProgs->spriteGLID);
    free(i_shaderProgs);
}

const ZF3ShaderProgs* zf3_get_shader_progs() {
    assert(i_shaderProgs);
    return i_shaderProgs;
}
