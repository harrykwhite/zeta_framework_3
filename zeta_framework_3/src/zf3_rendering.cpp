#include <zf3_rendering.h>

namespace zf3 {

static constexpr int ik_quadLimit = gk_spriteBatchSlotLimit;
static constexpr int ik_quadIndicesLen = 6 * ik_quadLimit;

static unsigned short i_quadIndices[ik_quadIndicesLen];
static int i_texUnits[gk_texUnitLimit];

static void clean_render_layers(Renderer* const renderer) {
    assert(renderer);

    for (int i = 0; i < renderer->layerCnt; ++i) {
        RenderLayer* const layer = &renderer->layers[i];

        for (int j = 0; j < layer->spriteBatchCnt; ++j) {
            glDeleteVertexArrays(1, &layer->spriteBatchGLIDs[j].vertArrayGLID);
            glDeleteBuffers(1, &layer->spriteBatchGLIDs[j].vertBufGLID);
            glDeleteBuffers(1, &layer->spriteBatchGLIDs[j].elemBufGLID);
        }
    }

    memset(renderer->layers, 0, sizeof(*renderer->layers) * renderer->layerCnt);

    renderer->layerCnt = 0;
}

static void add_sprite_batch(Renderer* const renderer, const int layerIndex) {
    assert(renderer);
    assert(layerIndex >= 0 && layerIndex < renderer->layerCnt);

    RenderLayer* const layer = &renderer->layers[layerIndex];
    const int batchIndex = layer->spriteBatchCnt;

    SpriteBatchGLIDs* const glIDs = &layer->spriteBatchGLIDs[batchIndex];

    // Generate vertex array.
    glGenVertexArrays(1, &glIDs->vertArrayGLID);
    glBindVertexArray(glIDs->vertArrayGLID);

    // Generate vertex buffer.
    glGenBuffers(1, &glIDs->vertBufGLID);
    glBindBuffer(GL_ARRAY_BUFFER, glIDs->vertBufGLID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * gk_spriteQuadShaderProgVertCnt * 4 * gk_spriteBatchSlotLimit, NULL, GL_DYNAMIC_DRAW);

    // Generate element buffer.
    glGenBuffers(1, &glIDs->elemBufGLID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIDs->elemBufGLID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 6 * gk_spriteBatchSlotLimit, i_quadIndices, GL_STATIC_DRAW);

    // Set vertex attribute pointers.
    const int vertsStride = sizeof(float) * gk_spriteQuadShaderProgVertCnt;

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vertsStride, (const void*)(sizeof(float) * 0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertsStride, (const void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertsStride, (const void*)(sizeof(float) * 4));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, vertsStride, (const void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, vertsStride, (const void*)(sizeof(float) * 7));
    glEnableVertexAttribArray(4);

    glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, vertsStride, (const void*)(sizeof(float) * 8));
    glEnableVertexAttribArray(5);

    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, vertsStride, (const void*)(sizeof(float) * 10));
    glEnableVertexAttribArray(6);

    // Unbind.
    glBindVertexArray(0);

    ++layer->spriteBatchCnt;
}

static int add_tex_unit_to_sprite_batch(SpriteBatchTransData* const batchTransData, const int texIndex) {
    for (int i = 0; i < batchTransData->texUnitsInUse; ++i) {
        if (batchTransData->texUnitTexIDs[i] == texIndex) {
            return i;
        }
    }

    if (batchTransData->texUnitsInUse == gk_texUnitLimit) {
        return -1;
    }

    batchTransData->texUnitTexIDs[batchTransData->texUnitsInUse] = texIndex;

    return batchTransData->texUnitsInUse++;
}

static void init_cam_view_matrix(Matrix4x4* const mat, const Camera* const cam, const WindowMeta* const windowMeta) {
    memset(mat, 0, sizeof(*mat));
    mat->elems[0][0] = cam->scale;
    mat->elems[1][1] = cam->scale;
    mat->elems[3][3] = 1.0f;
    mat->elems[3][0] = (-cam->pos.x * cam->scale) + (windowMeta->size.x / 2.0f);
    mat->elems[3][1] = (-cam->pos.y * cam->scale) + (windowMeta->size.y / 2.0f);
}

void load_render_layers(Renderer* const renderer, const int layerCnt, const int camLayerCnt) {
    assert(layerCnt > 0 && layerCnt <= gk_renderLayerLimit);
    assert(camLayerCnt >= 0 && camLayerCnt <= layerCnt);

    clean_render_layers(renderer);
    renderer->layerCnt = layerCnt;
    renderer->camLayerCnt = camLayerCnt;
}

void write_to_sprite_batch(Renderer* const renderer, const int layerIndex, const SpriteBatchWriteData* const writeData, const Assets* const assets) {
    assert(layerIndex >= 0 && layerIndex < renderer->layerCnt);

    RenderLayer* const layer = &renderer->layers[layerIndex];

    if (layer->spriteBatchCnt == 0) {
        add_sprite_batch(renderer, layerIndex);
    }

    const int batchIndex = layer->spriteBatchesFilled;
    SpriteBatchTransData* const batchTransData = &layer->spriteBatchTransDatas[batchIndex];

    int texUnit;

    if (batchTransData->slotsUsed == gk_spriteBatchSlotLimit || (texUnit = add_tex_unit_to_sprite_batch(batchTransData, writeData->texIndex)) == -1) {
        ++layer->spriteBatchesFilled;

        if (layer->spriteBatchesFilled == layer->spriteBatchCnt) {
            add_sprite_batch(renderer, layerIndex);
        }

        write_to_sprite_batch(renderer, layerIndex, writeData, assets);

        return;
    }

    const int slotIndex = batchTransData->slotsUsed;

    const Vec2DInt texSize = assets->texSizes[writeData->texIndex];

    const float verts[] = {
        (0.0f - writeData->origin.x) * writeData->scale.x,
        (0.0f - writeData->origin.y) * writeData->scale.y,
        writeData->pos.x,
        writeData->pos.y,
        static_cast<float>(writeData->srcRect.width),
        static_cast<float>(writeData->srcRect.height),
        writeData->rot,
        static_cast<float>(texUnit),
        static_cast<float>(writeData->srcRect.x) / texSize.x,
        static_cast<float>(writeData->srcRect.y) / texSize.y,
        writeData->alpha,

        (1.0f - writeData->origin.x) * writeData->scale.x,
        (0.0f - writeData->origin.y) * writeData->scale.y,
        writeData->pos.x,
        writeData->pos.y,
        static_cast<float>(writeData->srcRect.width),
        static_cast<float>(writeData->srcRect.height),
        writeData->rot,
        static_cast<float>(texUnit),
        static_cast<float>(writeData->srcRect.x + writeData->srcRect.width) / texSize.x,
        static_cast<float>(writeData->srcRect.y) / texSize.y,
        writeData->alpha,

        (1.0f - writeData->origin.x) * writeData->scale.x,
        (1.0f - writeData->origin.y) * writeData->scale.y,
        writeData->pos.x,
        writeData->pos.y,
        static_cast<float>(writeData->srcRect.width),
        static_cast<float>(writeData->srcRect.height),
        writeData->rot,
        static_cast<float>(texUnit),
        static_cast<float>(writeData->srcRect.x + writeData->srcRect.width) / texSize.x,
        static_cast<float>(writeData->srcRect.y + writeData->srcRect.height) / texSize.y,
        writeData->alpha,

        (0.0f - writeData->origin.x) * writeData->scale.x,
        (1.0f - writeData->origin.y) * writeData->scale.y,
        writeData->pos.x,
        writeData->pos.y,
        static_cast<float>(writeData->srcRect.width),
        static_cast<float>(writeData->srcRect.height),
        writeData->rot,
        static_cast<float>(texUnit),
        static_cast<float>(writeData->srcRect.x) / texSize.x,
        static_cast<float>(writeData->srcRect.y + writeData->srcRect.height) / texSize.y,
        writeData->alpha
    };

    const SpriteBatchGLIDs* const batchA = &layer->spriteBatchGLIDs[batchIndex];
    glBindVertexArray(batchA->vertArrayGLID);
    glBindBuffer(GL_ARRAY_BUFFER, batchA->vertBufGLID);
    glBufferSubData(GL_ARRAY_BUFFER, slotIndex * sizeof(float) * gk_spriteBatchSlotVertCnt, sizeof(verts), verts);

    ++batchTransData->slotsUsed;
}

void init_rendering_internals() {
    // Set quad indices (the same for all quad buffers).
    for (int i = 0; i < ik_quadLimit; i++) {
        i_quadIndices[(i * 6) + 0] = (i * 4) + 0;
        i_quadIndices[(i * 6) + 1] = (i * 4) + 1;
        i_quadIndices[(i * 6) + 2] = (i * 4) + 2;
        i_quadIndices[(i * 6) + 3] = (i * 4) + 2;
        i_quadIndices[(i * 6) + 4] = (i * 4) + 3;
        i_quadIndices[(i * 6) + 5] = (i * 4) + 0;
    }

    // Set texture units.
    for (int i = 0; i < gk_texUnitLimit; ++i) {
        i_texUnits[i] = i;
    }
}

void clean_renderer(Renderer* const renderer) {
    clean_render_layers(renderer);
    memset(renderer, 0, sizeof(*renderer));
}

void render_sprite_batches(const Renderer* const renderer, const Camera* const cam, const ShaderProgs* const shaderProgs, const WindowMeta* const windowMeta, const Assets* const assets) {
    assert(!cam == !renderer->camLayerCnt);

    Matrix4x4 projMat;
    init_ortho_matrix_4x4(&projMat, 0.0f, windowMeta->size.x, windowMeta->size.y, 0.0f, -1.0f, 1.0f);

    Matrix4x4 camViewMat;
    init_cam_view_matrix(&camViewMat, cam, windowMeta);

    Matrix4x4 defaultViewMat;
    init_identity_matrix_4x4(&defaultViewMat);

    for (int i = 0; i < renderer->layerCnt; ++i) {
        glUseProgram(shaderProgs->spriteQuadGLID);

        glUniformMatrix4fv(shaderProgs->spriteQuadProjUniLoc, 1, GL_FALSE, (const float*)projMat.elems);

        const Matrix4x4* const viewMat = i < renderer->camLayerCnt ? &camViewMat : &defaultViewMat; // TODO: Pull this check out of the loop.
        glUniformMatrix4fv(shaderProgs->spriteQuadViewUniLoc, 1, GL_FALSE, (const float*)viewMat->elems);

        glUniform1iv(shaderProgs->spriteQuadTexturesUniLoc, gk_texUnitLimit, i_texUnits);

        const RenderLayer* const layer = &renderer->layers[i];

        for (int j = 0; j < layer->spriteBatchCnt; ++j) {
            const SpriteBatchGLIDs* const batchGLIDs = &layer->spriteBatchGLIDs[j];
            const SpriteBatchTransData* const batchTransData = &layer->spriteBatchTransDatas[j];

            glBindVertexArray(batchGLIDs->vertArrayGLID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batchGLIDs->elemBufGLID);

            for (int k = 0; k < batchTransData->texUnitsInUse; ++k) {
                glActiveTexture(GL_TEXTURE0 + i_texUnits[k]);
                glBindTexture(GL_TEXTURE_2D, assets->texGLIDs[batchTransData->texUnitTexIDs[k]]);
            }

            glDrawElements(GL_TRIANGLES, 6 * batchTransData->slotsUsed, GL_UNSIGNED_SHORT, NULL);
        }
    }
}

void empty_sprite_batches(Renderer* const renderer) {
    for (int i = 0; i < renderer->layerCnt; ++i) {
        RenderLayer* const layer = &renderer->layers[i];
        memset(layer->spriteBatchTransDatas, 0, sizeof(layer->spriteBatchTransDatas));
        layer->spriteBatchesFilled = 0;
    }
}

}
