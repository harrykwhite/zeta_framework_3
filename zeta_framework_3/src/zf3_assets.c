#include <zf3_local.h>

static ZF3Assets* i_assets;
static ZF3ShaderProgs* i_shaderProgs;

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

    if (i_assets->texCnt > 0) {
        // Allocate a buffer for temporarily storing the pixel data of each texture.
        const int pxDataBufSize = ZF3_TEX_CHANNEL_COUNT * ZF3_TEX_WIDTH_LIMIT * ZF3_TEX_HEIGHT_LIMIT;
        unsigned char* const pxDataBuf = malloc(pxDataBufSize);

        if (!pxDataBuf) {
            return false;
        }

        // Generate textures and store their IDs.
        glGenTextures(i_assets->texCnt, i_assets->texGLIDs);

        // Read the sizes and pixel data of textures and finish setting them up.
        for (int i = 0; i < i_assets->texCnt; ++i) {
            fread(&i_assets->texSizes[i], sizeof(i_assets->texSizes[i]), 1, fs);

            fread(pxDataBuf, ZF3_TEX_CHANNEL_COUNT * i_assets->texSizes[i].x * i_assets->texSizes[i].y, 1, fs);

            glBindTexture(GL_TEXTURE_2D, i_assets->texGLIDs[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i_assets->texSizes[i].x, i_assets->texSizes[i].y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pxDataBuf);
        }

        free(pxDataBuf);
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

    i_shaderProgs->spriteQuadGLID = create_shader_prog_from_srcs(i_spriteQuadVertShaderSrc, i_spriteQuadFragShaderSrc);
    assert(i_shaderProgs->spriteQuadGLID);

    i_shaderProgs->spriteQuadProjUniLoc = glGetUniformLocation(i_shaderProgs->spriteQuadGLID, "u_proj");
    i_shaderProgs->spriteQuadViewUniLoc = glGetUniformLocation(i_shaderProgs->spriteQuadGLID, "u_view");
    i_shaderProgs->spriteQuadTexturesUniLoc = glGetUniformLocation(i_shaderProgs->spriteQuadGLID, "u_textures");

    return true;
}

void zf3_unload_shader_progs() {
    assert(i_shaderProgs);
    glDeleteProgram(i_shaderProgs->spriteQuadGLID);
    free(i_shaderProgs);
}

const ZF3ShaderProgs* zf3_get_shader_progs() {
    assert(i_shaderProgs);
    return i_shaderProgs;
}
