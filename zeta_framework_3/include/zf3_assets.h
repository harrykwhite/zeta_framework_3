#pragma once

#include <assert.h>
#include <AL/alext.h>
#include <zf3c.h>
#include <zf3_misc.h>

namespace zf3 {
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

    struct Sounds {
        int cnt;
        ALID bufALIDs[gk_fontLimit];
    };

    struct Assets {
        Textures textures;
        Fonts fonts;
        Sounds sounds;
    };

    bool init_assets(Assets* const assets);
    void clean_assets(Assets* const assets);
}
