#pragma once

#include <zf3c.h>
#include <zf3_window.h>
#include <zf3_assets.h>
#include <zf3_misc.h>

namespace zf3 {

constexpr int gk_renderLayerLimit = 32;
constexpr int gk_renderLayerSpriteBatchLimit = 256;
constexpr int gk_spriteBatchSlotLimit = 4096;
constexpr int gk_spriteBatchSlotVertCnt = gk_spriteQuadShaderProgVertCnt * 4;
constexpr int gk_texUnitLimit = 16; // This is the minimum guaranteed by OpenGL. For now, we don't consider any higher than this.

struct Color {
    float r;
    float g;
    float b;
    float a;
};

struct SpriteBatchGLIDs {
    GLID vertArrayGLID;
    GLID vertBufGLID;
    GLID elemBufGLID;
};

struct SpriteBatchTransData {
    int slotsUsed;
    int texUnitTexIDs[gk_texUnitLimit];
    int texUnitsInUse;
};

struct SpriteBatchWriteData {
    int texIndex;
    Vec2D pos;
    Rect srcRect;
    Vec2D origin;
    float rot;
    Vec2D scale;
    float alpha;
};

struct RenderLayer {
    SpriteBatchGLIDs spriteBatchGLIDs[gk_renderLayerSpriteBatchLimit];
    SpriteBatchTransData spriteBatchTransDatas[gk_renderLayerSpriteBatchLimit]; // Cleared on emptying.
    int spriteBatchesFilled; // Cleared on emptying.
    int spriteBatchCnt;
};

struct Renderer {
    RenderLayer layers[gk_renderLayerLimit];
    int layerCnt;
    int camLayerCnt; // Layers 0 through to this number are drawn with a camera view matrix.

    Color bgColor;
};

struct Camera {
    Vec2D pos;
    float scale;
};

void init_rendering_internals();
void clean_renderer(Renderer* const renderer);
void load_render_layers(Renderer* const renderer, const int layerCnt, const int camLayerCnt);
void render_all(const Renderer* const renderer, const Camera* const cam, const ShaderProgs* const shaderProgs, const WindowMeta* const windowMeta, const Assets* const assets);
void empty_sprite_batches(Renderer* const renderer);
void write_to_sprite_batch(Renderer* const renderer, const int layerIndex, const SpriteBatchWriteData* const writeData, const Assets* const assets);

inline Vec2D conv_camera_to_screen_pos(const Vec2D pos, const Camera* const cam, const WindowMeta* const windowMeta) {
    return Vec2D {
        .x = ((pos.x - cam->pos.x) * cam->scale) + (windowMeta->size.x / 2.0f),
        .y = ((pos.y - cam->pos.y) * cam->scale) + (windowMeta->size.y / 2.0f)
    };
}

inline Vec2D conv_screen_to_camera_pos(const Vec2D pos, const Camera* const cam, const WindowMeta* const windowMeta) {
    return Vec2D {
        .x = ((pos.x - (windowMeta->size.x / 2.0f)) / cam->scale) + cam->pos.x,
        .y = ((pos.y - (windowMeta->size.y / 2.0f)) / cam->scale) + cam->pos.y
    };
}

}
