#include <zf3_shader_progs.h>

namespace zf3 {
    static GLID create_shader_from_src(const char* const src, const bool frag) {
        const GLID glID = glCreateShader(frag ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER);
        glShaderSource(glID, 1, &src, nullptr);
        glCompileShader(glID);

        GLint compileSuccess;
        glGetShaderiv(glID, GL_COMPILE_STATUS, &compileSuccess);

        if (!compileSuccess) {
            GLchar infoLog[512];
            glGetShaderInfoLog(glID, sizeof(infoLog), nullptr, infoLog);
            log_error("Failed to compile shader!\n\n%s\n", infoLog);

            glDeleteShader(glID);

            return 0;
        }

        return glID;
    }

    static GLID create_shader_prog_from_srcs(const char* const vertShaderSrc, const char* const fragShaderSrc) {
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

    static SpriteQuadShaderProg load_sprite_quad_shader_prog() {
        const char* const vertShaderSrc =
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

        const char* const fragShaderSrc =
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

        const GLID glID = create_shader_prog_from_srcs(vertShaderSrc, fragShaderSrc);
        assert(glID);

        return {
            .glID = glID,
            .projUniLoc = glGetUniformLocation(glID, "u_proj"),
            .viewUniLoc = glGetUniformLocation(glID, "u_view"),
            .texturesUniLoc = glGetUniformLocation(glID, "u_textures")
        };
    }

    static CharQuadShaderProg load_char_quad_shader_prog() {
        const char* const vertShaderSrc =
            "#version 430 core\n"
            "\n"
            "layout (location = 0) in vec2 a_vert;\n"
            "layout (location = 1) in vec2 a_texCoord;\n"
            "\n"
            "out vec2 v_texCoord;\n"
            "\n"
            "uniform vec2 u_pos;\n"
            "uniform float u_rot;\n"
            "\n"
            "uniform mat4 u_proj;\n"
            "uniform mat4 u_view;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    float rotCos = cos(u_rot);\n"
            "    float rotSin = sin(u_rot);\n"
            "\n"
            "    mat4 model = mat4(\n"
            "        vec4(rotCos, rotSin, 0.0f, 0.0f),\n"
            "        vec4(-rotSin, rotCos, 0.0f, 0.0f),\n"
            "        vec4(0.0f, 0.0f, 1.0f, 0.0f),\n"
            "        vec4(u_pos.x, u_pos.y, 0.0f, 1.0f)\n"
            "    );\n"
            "\n"
            "    gl_Position = u_proj * u_view * model * vec4(a_vert, 0.0f, 1.0f);\n"
            "\n"
            "    v_texCoord = a_texCoord;\n"
            "}\n";

        const char* const fragShaderSrc =
            "#version 430 core\n"
            "\n"
            "in vec2 v_texCoord;\n"
            "\n"
            "out vec4 o_fragColor;\n"
            "\n"
            "uniform vec4 u_blend;\n"
            "uniform sampler2D u_tex;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec4 texColor = texture(u_tex, v_texCoord);\n"
            "    o_fragColor = texColor * u_blend;\n"
            "}\n";

        const GLID glID = create_shader_prog_from_srcs(vertShaderSrc, fragShaderSrc);
        assert(glID);

        return {
            .glID = glID,
            .projUniLoc = glGetUniformLocation(glID, "u_proj"),
            .viewUniLoc = glGetUniformLocation(glID, "u_view"),
            .posUniLoc = glGetUniformLocation(glID, "u_pos"),
            .rotUniLoc = glGetUniformLocation(glID, "u_rot"),
            .blendUniLoc = glGetUniformLocation(glID, "u_blend")
        };
    }

    ShaderProgs load_shader_progs() {
        return {
            .spriteQuad = load_sprite_quad_shader_prog(),
            .charQuad = load_char_quad_shader_prog()
        };
    }

    void unload_shader_progs(ShaderProgs& progs) {
        if (progs.spriteQuad.glID) {
            glDeleteProgram(progs.charQuad.glID);
        }

        if (progs.charQuad.glID) {
            glDeleteProgram(progs.spriteQuad.glID);
        }

        zero_out(progs);
    }
}
