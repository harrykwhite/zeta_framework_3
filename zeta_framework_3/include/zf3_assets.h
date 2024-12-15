#pragma once

#include <assert.h>
#include <zf3c.h>
#include <zf3_misc.h>

namespace zf3 {

constexpr int gk_spriteQuadShaderProgVertCnt = 11;

struct Textures {
    int cnt;
    GLID glIDs[gk_texLimit];
    Vec2DInt sizes[gk_texLimit];
};

struct Fonts {
    int cnt;
    FontArrangementInfo arrangementInfos[gk_fontLimit];
    GLID texGLIDs[gk_fontLimit];
    Vec2DInt texSizes[gk_fontLimit];
};

struct Assets {
    Textures textures;
    Fonts fonts;
};

bool load_assets();
void unload_assets();
const Assets& get_assets();

bool load_textures(Textures& textures, FILE* const fs);
void unload_textures(Textures& textures);

bool load_fonts(Fonts& fonts, FILE* const fs);
void unload_fonts(Fonts& fonts);

}
