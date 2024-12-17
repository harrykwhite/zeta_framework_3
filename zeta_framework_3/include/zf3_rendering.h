#pragma once

#include <zf3c.h>
#include <zf3_window.h>
#include <zf3_assets.h>
#include <zf3_misc.h>

namespace zf3 {
    //
    // Shader Programs
    //
    constexpr int gk_spriteQuadShaderProgVertCnt = 11;
    constexpr int gk_charQuadShaderProgVertCnt = 4;

    struct SpriteQuadShaderProg {
        GLID glID;
        int projUniLoc;
        int viewUniLoc;
        int texturesUniLoc;
    };

    struct CharQuadShaderProg {
        GLID glID;
        int projUniLoc;
        int viewUniLoc;
        int posUniLoc;
        int rotUniLoc;
        int blendUniLoc;
    };

    struct ShaderProgs {
        SpriteQuadShaderProg spriteQuad;
        CharQuadShaderProg charQuad;
    };

    ShaderProgs load_shader_progs();
    void clean_shader_progs(ShaderProgs& progs);

    //
    // Renderer
    //
    static constexpr int gk_renderLayerLimit = 32;
    static constexpr int gk_renderLayerSpriteBatchLimit = 256;
    static constexpr int gk_renderLayerCharBatchLimit = 256;
    static constexpr int gk_spriteBatchSlotLimit = 4096;
    static constexpr int gk_charBatchSlotLimit = 1024;
    static constexpr int gk_texUnitLimit = 16;

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
        Vec4D blend;
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

        Vec3D bgColor;
        Camera cam;
    };

    void clean_renderer(Renderer& renderer);
    void reset_renderer(Renderer& renderer, const int layerCnt, const int camLayerCnt = 0, const Vec3D bgColor = {}, const Vec2D camPos = {}, const float camScale = 2.0f);
    void render_all(const Renderer& renderer, const ShaderProgs& shaderProgs, const Window& window);

    void empty_sprite_batches(Renderer& renderer);
    void write_to_sprite_batch(Renderer& renderer, const int layerIndex, const int texIndex, const Vec2D pos, const Rect& srcRect, const Vec2D origin = {0.5f, 0.5f}, const float rot = 0.0f, const Vec2D scale = {1.0f, 1.0f}, const float alpha = 1.0f);

    CharBatchID activate_any_char_batch(Renderer& renderer, const int layerIndex, const int slotCnt, const int fontIndex, const Vec2D pos);
    void deactivate_char_batch(Renderer& renderer, const CharBatchID id);
    void write_to_char_batch(Renderer& renderer, const CharBatchID id, const char* const text, const FontHorAlign horAlign, const FontVerAlign verAlign);
    void clear_char_batch(const Renderer& renderer, const CharBatchID id);

    inline CharBatchDisplayProps& get_char_batch_display_props(Renderer& renderer, const CharBatchID id) {
        return renderer.layers[id.layerIndex].charBatches[id.batchIndex].displayProps;
    }

    inline Vec2D camera_to_screen_pos(const Vec2D pos, const Camera& cam, const Window& window) {
        return {
            (pos.x + (cam.pos.x * cam.scale)) - (window.size.x / 2.0f),
            (pos.y + (cam.pos.y * cam.scale)) - (window.size.y / 2.0f)
        };
    }

    inline Vec2D screen_to_camera_pos(const Vec2D pos, const Camera& cam, const Window& window) {
        return {
            (pos.x + (window.size.x / 2.0f)) / cam.scale - cam.pos.x,
            (pos.y + (window.size.y / 2.0f)) / cam.scale - cam.pos.y
        };
    }
}
