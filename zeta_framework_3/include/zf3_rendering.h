#pragma once

#include <zf3c.h>
#include <zf3_window.h>
#include <zf3_assets.h>
#include <zf3_misc.h>

namespace zf3 {

struct Color {
    float r;
    float g;
    float b;
    float a;
};

extern Color g_bgColor;

struct Camera {
    Vec2D pos;
    float scale;
};

extern Camera g_camera;

void init_rendering_internals();
void rendering_cleanup();
void load_render_layers(const int layerCnt, const int camLayerCnt);
void render_all();
void empty_sprite_batches();
void write_to_sprite_batch(const int layerIndex, const int texIndex, const Vec2D pos, const Rect srcRect, const Vec2D origin = {0.5f, 0.5f}, const float rot = 0.0f, const Vec2D scale = {1.0f, 1.0f}, const float alpha = 1.0f);

GLID create_shader_from_src(const char* const src, const bool frag);
GLID create_shader_prog_from_srcs(const char* const vertShaderSrc, const char* const fragShaderSrc);

inline Vec2D camera_to_screen_pos(const Vec2D pos) {
    return {
        .x = ((pos.x - g_camera.pos.x) * g_camera.scale) + (get_window_size().x / 2.0f),
        .y = ((pos.y - g_camera.pos.y) * g_camera.scale) + (get_window_size().y / 2.0f)
    };
}

inline Vec2D screen_to_camera_pos(const Vec2D pos) {
    return {
        .x = ((pos.x - (get_window_size().x / 2.0f)) / g_camera.scale) + g_camera.pos.x,
        .y = ((pos.y - (get_window_size().y / 2.0f)) / g_camera.scale) + g_camera.pos.y
    };
}

}
