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

extern Color g_bgColor;

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

struct Camera {
    Vec2D pos;
    float scale;
};

void init_rendering_internals();
void rendering_cleanup();
void load_render_layers(const int layerCnt, const int camLayerCnt);
void render_all();
void empty_sprite_batches();
void write_to_sprite_batch(const int layerIndex, const SpriteBatchWriteData* const writeData);

GLID create_shader_from_src(const char* const src, const bool frag);
GLID create_shader_prog_from_srcs(const char* const vertShaderSrc, const char* const fragShaderSrc);

inline Vec2D camera_to_screen_pos(const Vec2D pos, const Camera* const cam) {
    return Vec2D {
        .x = ((pos.x - cam->pos.x) * cam->scale) + (get_window_size().x / 2.0f),
        .y = ((pos.y - cam->pos.y) * cam->scale) + (get_window_size().y / 2.0f)
    };
}

inline Vec2D screen_to_camera_pos(const Vec2D pos, const Camera* const cam) {
    return Vec2D {
        .x = ((pos.x - (get_window_size().x / 2.0f)) / cam->scale) + cam->pos.x,
        .y = ((pos.y - (get_window_size().y / 2.0f)) / cam->scale) + cam->pos.y
    };
}

}
