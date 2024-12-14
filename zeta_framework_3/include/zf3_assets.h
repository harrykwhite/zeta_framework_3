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

bool load_assets();
void unload_assets();
const Assets& get_assets();

}
