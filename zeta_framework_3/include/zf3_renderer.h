#pragma once

#include <zf3c.h>
#include <zf3_window.h>
#include <zf3_assets.h>
#include <zf3_shader_progs.h>
#include <zf3_misc.h>

namespace zf3 {
    constexpr int gk_renderLayerLimit = 32;
    constexpr int gk_renderLayerSpriteBatchLimit = 256;
    constexpr int gk_renderLayerCharBatchLimit = 256;
    constexpr int gk_spriteBatchSlotLimit = 4096;
    constexpr int gk_charBatchSlotLimit = 1024;
    constexpr int gk_texUnitLimit = 16;

    enum FontHorAlign {
        FONT_HOR_ALIGN_LEFT,
        FONT_HOR_ALIGN_CENTER,
        FONT_HOR_ALIGN_RIGHT
    };

    enum FontVerAlign {
        FONT_VER_ALIGN_TOP,
        FONT_VER_ALIGN_CENTER,
        FONT_VER_ALIGN_BOTTOM
    };

    struct Color {
        float r, g, b, a;
    };

    inline const Color gk_white = {1.0f, 1.0f, 1.0f};
    inline const Color gk_black = {0.0f, 0.0f, 0.0f};
    inline const Color gk_red = {1.0f, 0.0f, 0.0f};
    inline const Color gk_green = {0.0f, 1.0f, 0.0f};
    inline const Color gk_blue = {0.0f, 0.0f, 1.0f};

    struct QuadBuf {
        GLID vertArrayGLID;
        GLID vertBufGLID;
        GLID elemBufGLID;
    };

    struct SpriteBatchTransData {
        int slotsUsed;
        int texUnitTexIDs[gk_texUnitLimit];
        int texUnitsInUse;
    };

    struct CharBatchDisplayProps {
        int fontIndex;
        Vec2D pos;
        float rot;
        Color blend;
    };

    struct CharBatch {
        QuadBuf quadBuf;
        int slotCnt;
        CharBatchDisplayProps displayProps;
    };

    struct CharBatchID {
        int layerIndex;
        int batchIndex;
    };

    struct RenderLayer {
        QuadBuf spriteBatchQuadBufs[gk_renderLayerSpriteBatchLimit];
        SpriteBatchTransData spriteBatchTransDatas[gk_renderLayerSpriteBatchLimit];
        int spriteBatchesFilled;
        int spriteBatchCnt;
        CharBatch charBatches[gk_renderLayerCharBatchLimit];
        Bitset<gk_renderLayerCharBatchLimit> charBatchActivity;
    };

    struct Camera {
        Vec2D pos;
        float scale;
    };

    struct Renderer {
        RenderLayer layers[gk_renderLayerLimit];
        int layerCnt;
        int camLayerCnt;

        Color bgColor;
        Camera cam;
    };

    void clean_renderer(Renderer& renderer);
    void reset_renderer(Renderer& renderer, const int layerCnt, const int camLayerCnt = 0, const Color bgColor = {}, const Vec2D camPos = {}, const float camScale = 2.0f);
    void render_all(const Renderer& renderer, const ShaderProgs& shaderProgs);

    void empty_sprite_batches(Renderer& renderer);
    void write_to_sprite_batch(Renderer& renderer, const int layerIndex, const int texIndex, const Vec2D pos, const Rect& srcRect, const Vec2D origin = {0.5f, 0.5f}, const float rot = 0.0f, const Vec2D scale = {1.0f, 1.0f}, const float alpha = 1.0f);

    CharBatchID activate_any_char_batch(Renderer& renderer, const int layerIndex, const int slotCnt, const int fontIndex, const Vec2D pos);
    void deactivate_char_batch(Renderer& renderer, const CharBatchID id);
    void write_to_char_batch(Renderer& renderer, const CharBatchID id, const char* const text, const FontHorAlign horAlign, const FontVerAlign verAlign);
    void clear_char_batch(const Renderer& renderer, const CharBatchID id);

    inline CharBatchDisplayProps& get_char_batch_display_props(Renderer& renderer, const CharBatchID id) {
        return renderer.layers[id.layerIndex].charBatches[id.batchIndex].displayProps;
    }

    inline Vec2D get_camera_top_left(const Camera& cam) {
        return cam.pos - (to_vec_2d(get_window_size()) / (cam.scale * 2.0f));
    }

    inline Pt2D get_camera_size(const Camera& cam) {
        return {
            static_cast<int>(get_window_size().x / cam.scale),
            static_cast<int>(get_window_size().y / cam.scale)
        };
    }

    inline Vec2D camera_to_screen_pos(const Vec2D pos, const Camera& cam) {
        return (pos - get_camera_top_left(cam)) * cam.scale;
    }

    inline Vec2D screen_to_camera_pos(const Vec2D pos, const Camera& cam) {
        return get_camera_top_left(cam) + (pos / cam.scale);
    }
}
