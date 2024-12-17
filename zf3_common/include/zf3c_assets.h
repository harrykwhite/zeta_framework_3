#pragma once

#include <zf3c_math.h>

namespace zf3 {
    constexpr const char* gk_assetsFileName = "assets.zf3";

    constexpr int gk_texLimit = 512;
    constexpr int gk_fontLimit = 64;
    constexpr int gk_soundLimit = 256;
    constexpr int gk_musicLimit = 40;

    constexpr int gk_texChannelCnt = 4;
    constexpr Vec2DInt gk_texSizeLimit = {2048, 2048};
    constexpr int gk_texPxDataSizeLimit = gk_texChannelCnt * gk_texSizeLimit.x * gk_texSizeLimit.y;

    constexpr int gk_fontCharRangeBegin = 32;
    constexpr int gk_fontCharRangeSize = 95;

    constexpr int gk_soundSampleLimit = 441000;

    using AudioSample = float;

    struct FontCharsArrangementInfo {
        int horOffsets[gk_fontCharRangeSize];
        int verOffsets[gk_fontCharRangeSize];
        int horAdvances[gk_fontCharRangeSize];

        Rect srcRects[gk_fontCharRangeSize];

        int kernings[gk_fontCharRangeSize * gk_fontCharRangeSize];
    };

    struct FontArrangementInfo {
        int lineHeight;
        FontCharsArrangementInfo chars;
    };

    struct AudioInfo {
        int channelCnt;
        long long sampleCntPerChannel;
        int sampleRate;
    };
}
