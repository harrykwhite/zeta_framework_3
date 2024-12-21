#pragma once

#include <assert.h>
#include <AL/alext.h>
#include <zf3c.h>
#include <zf3_misc.h>

namespace zf3 {
    struct Textures {
        int cnt;
        GLID glIDs[gk_texLimit];
        Pt2D sizes[gk_texLimit];
    };

    struct Fonts {
        int cnt;
        FontArrangementInfo arrangementInfos[gk_fontLimit];
        GLID texGLIDs[gk_fontLimit];
        Pt2D texSizes[gk_fontLimit];
    };

    struct Sounds {
        int cnt;
        ALID bufALIDs[gk_soundLimit];
    };

    struct Music {
        int cnt;
        int sampleDataFilePositions[gk_musicLimit];
        AudioInfo infos[gk_musicLimit];
    };

    struct Assets {
        Textures textures;
        Fonts fonts;
        Sounds sounds;
        Music music;
    };

    bool load_assets();
    void unload_assets();
    const Assets& get_assets();
}
