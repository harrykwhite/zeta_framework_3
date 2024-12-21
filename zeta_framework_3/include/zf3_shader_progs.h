#pragma once

#include <zf3c.h>
#include <zf3_window.h>
#include <zf3_assets.h>
#include <zf3_misc.h>

namespace zf3 {
    constexpr int gk_spriteQuadShaderProgVertCnt = 11;
    constexpr int gk_charQuadShaderProgVertCnt = 4;

    struct SpriteQuadShaderProg {
        GLID glID;
        int projUniLoc;
        int viewUniLoc;
        int texturesUniLoc;
    };

    struct CharQuadShaderProg {
        GLID glID;
        int projUniLoc;
        int viewUniLoc;
        int posUniLoc;
        int rotUniLoc;
        int blendUniLoc;
    };

    struct ShaderProgs {
        SpriteQuadShaderProg spriteQuad;
        CharQuadShaderProg charQuad;
    };

    ShaderProgs load_shader_progs();
    void unload_shader_progs(ShaderProgs& progs);
}
