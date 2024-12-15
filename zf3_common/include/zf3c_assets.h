#pragma once

#include <zf3c_math.h>

namespace zf3 {

constexpr const char* gk_assetsFileName = "assets.zf3";

constexpr int gk_texChannelCnt = 4;
constexpr Vec2DInt gk_texSizeLimit = {2048, 2048};

constexpr int gk_fontCharRangeBegin = 32;
constexpr int gk_fontCharRangeSize = 95;

struct FontCharsDisplayInfo {
    int horOffsets[gk_fontCharRangeSize];
    int verOffsets[gk_fontCharRangeSize];
    int horAdvances[gk_fontCharRangeSize];

    Rect srcRects[gk_fontCharRangeSize];

    int kernings[gk_fontCharRangeSize * gk_fontCharRangeSize];
};

struct FontDisplayInfo {
    int lineHeight;
    FontCharsDisplayInfo chars;
    Vec2DInt texSize;
};

}
