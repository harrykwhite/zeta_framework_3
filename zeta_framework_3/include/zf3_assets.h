#pragma once

#include <assert.h>
#include <zf3c.h>
#include <zf3_misc.h>

namespace zf3 {

constexpr int gk_spriteQuadShaderProgVertCnt = 11;

constexpr int gk_texLimit = 256;

struct Assets {
    int texCnt;
    GLID texGLIDs[gk_texLimit];
    Vec2DInt texSizes[gk_texLimit];
};

struct ShaderProgs {
    GLID spriteQuadGLID;
    int spriteQuadProjUniLoc;
    int spriteQuadViewUniLoc;
    int spriteQuadTexturesUniLoc;
};

bool load_assets(Assets* const assets);
void unload_assets(Assets* const assets);

void load_shader_progs(ShaderProgs* const progs);
void unload_shader_progs(ShaderProgs* const progs);

}
