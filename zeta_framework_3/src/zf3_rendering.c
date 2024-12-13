#include <zf3_local.h>

#define QUAD_LIMIT ZF3_SPRITE_BATCH_SLOT_LIMIT
#define QUAD_INDICES_LEN (6 * QUAD_LIMIT)

static unsigned short i_quadIndices[QUAD_INDICES_LEN];
static int i_texUnits[ZF3_TEX_UNIT_LIMIT];

ZF3Renderer i_renderer;

static void add_sprite_batch() {
    const int batchIndex = i_renderer.spriteBatchCnt;

    ZF3SpriteBatchGLIDs* const glIDs = &i_renderer.spriteBatchGLIDs[batchIndex];

    // Generate vertex array.
    glGenVertexArrays(1, &glIDs->vertArrayGLID);
    glBindVertexArray(glIDs->vertArrayGLID);

    // Generate vertex buffer.
    glGenBuffers(1, &glIDs->vertBufGLID);
    glBindBuffer(GL_ARRAY_BUFFER, glIDs->vertBufGLID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ZF3_SPRITE_QUAD_SHADER_PROG_VERT_CNT * 4 * ZF3_SPRITE_BATCH_SLOT_LIMIT, NULL, GL_DYNAMIC_DRAW);

    // Generate element buffer.
    glGenBuffers(1, &glIDs->elemBufGLID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIDs->elemBufGLID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 6 * ZF3_SPRITE_BATCH_SLOT_LIMIT, i_quadIndices, GL_STATIC_DRAW);

    // Set vertex attribute pointers.
    const int vertsStride = sizeof(float) * ZF3_SPRITE_QUAD_SHADER_PROG_VERT_CNT;

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

    ++i_renderer.spriteBatchCnt;
}

static int add_tex_unit_to_sprite_batch(ZF3SpriteBatchTransData* const batchB, const int texIndex) {
    for (int i = 0; i < batchB->texUnitsInUse; ++i) {
        if (batchB->texUnitTexIDs[i] == texIndex) {
            return i;
        }
    }

    if (batchB->texUnitsInUse == ZF3_TEX_UNIT_LIMIT) {
        return -1;
    }

    batchB->texUnitTexIDs[batchB->texUnitsInUse] = texIndex;

    return batchB->texUnitsInUse++;
}

void zf3_init_rendering_internals() {
    // Set quad indices (the same for all quad buffers).
    for (int i = 0; i < QUAD_LIMIT; i++) {
        i_quadIndices[(i * 6) + 0] = (i * 4) + 0;
        i_quadIndices[(i * 6) + 1] = (i * 4) + 1;
        i_quadIndices[(i * 6) + 2] = (i * 4) + 2;
        i_quadIndices[(i * 6) + 3] = (i * 4) + 2;
        i_quadIndices[(i * 6) + 4] = (i * 4) + 3;
        i_quadIndices[(i * 6) + 5] = (i * 4) + 0;
    }

    // Set texture units.
    for (int i = 0; i < ZF3_TEX_UNIT_LIMIT; ++i) {
        i_texUnits[i] = i;
    }
}

void zf3_clean_renderer() {
    for (int i = 0; i < i_renderer.spriteBatchCnt; ++i) {
        glDeleteVertexArrays(1, &i_renderer.spriteBatchGLIDs[i].vertArrayGLID);
        glDeleteBuffers(1, &i_renderer.spriteBatchGLIDs[i].vertBufGLID);
        glDeleteBuffers(1, &i_renderer.spriteBatchGLIDs[i].elemBufGLID);
    }

    memset(&i_renderer, 0, sizeof(i_renderer));
}

void zf3_render_sprite_batches() {
    const ZF3ShaderProgs* const progs = zf3_get_shader_progs();

    glUseProgram(progs->spriteQuadGLID);

    ZF3Matrix4x4 projMat;
    zf3_init_ortho_matrix_4x4(&projMat, 0.0f, zf3_get_window_size().x, zf3_get_window_size().y, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(progs->spriteQuadProjUniLoc, 1, GL_FALSE, (const float*)projMat.elems);

    ZF3Matrix4x4 viewMat;
    zf3_init_identity_matrix_4x4(&viewMat);
    glUniformMatrix4fv(progs->spriteQuadViewUniLoc, 1, GL_FALSE, (const float*)viewMat.elems);

    glUniform1iv(progs->spriteQuadTexturesUniLoc, ZF3_TEX_UNIT_LIMIT, i_texUnits);

    for (int i = 0; i < i_renderer.spriteBatchCnt; ++i) {
        const ZF3SpriteBatchGLIDs* const batchGLIDs = &i_renderer.spriteBatchGLIDs[i];
        const ZF3SpriteBatchTransData* const batchTransData = &i_renderer.spriteBatchTransDatas[i];

        glBindVertexArray(batchGLIDs->vertArrayGLID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batchGLIDs->elemBufGLID);

        for (int j = 0; j < batchTransData->texUnitsInUse; ++j) {
            glActiveTexture(GL_TEXTURE0 + i_texUnits[j]);
            glBindTexture(GL_TEXTURE_2D, zf3_get_assets()->texGLIDs[batchTransData->texUnitTexIDs[j]]);
        }

        glDrawElements(GL_TRIANGLES, 6 * batchTransData->slotsUsed, GL_UNSIGNED_SHORT, NULL);
    }
}

void zf3_empty_sprite_batches() {
    memset(i_renderer.spriteBatchTransDatas, 0, sizeof(i_renderer.spriteBatchTransDatas));
    i_renderer.spriteBatchesFilled = 0;
}

void zf3_write_to_sprite_batch(const ZF3SpriteBatchWriteData* const writeData) {
    if (!i_renderer.spriteBatchCnt) {
        add_sprite_batch(i_renderer);
    }

    const int batchIndex = i_renderer.spriteBatchesFilled;
    ZF3SpriteBatchTransData* const batchTransData = &i_renderer.spriteBatchTransDatas[batchIndex];

    int texUnit;

    if (batchTransData->slotsUsed == ZF3_SPRITE_BATCH_SLOT_LIMIT || (texUnit = add_tex_unit_to_sprite_batch(batchTransData, writeData->texIndex)) == -1) {
        ++i_renderer.spriteBatchesFilled;

        if (i_renderer.spriteBatchesFilled == i_renderer.spriteBatchCnt) {
            add_sprite_batch(i_renderer);
        }

        zf3_write_to_sprite_batch(writeData);

        return;
    }

    const int slotIndex = batchTransData->slotsUsed;

    const ZF3Vec2DInt texSize = zf3_get_assets()->texSizes[writeData->texIndex];

    const float verts[] = {
        (0.0f - writeData->origin.x) * writeData->scale.x,
        (0.0f - writeData->origin.y) * writeData->scale.y,
        writeData->pos.x,
        writeData->pos.y,
        writeData->srcRect.width,
        writeData->srcRect.height,
        writeData->rot,
        texUnit,
        (float)writeData->srcRect.x / texSize.x,
        (float)writeData->srcRect.y / texSize.y,
        writeData->alpha,

        (1.0f - writeData->origin.x) * writeData->scale.x,
        (0.0f - writeData->origin.y) * writeData->scale.y,
        writeData->pos.x,
        writeData->pos.y,
        writeData->srcRect.width,
        writeData->srcRect.height,
        writeData->rot,
        texUnit,
        (float)(writeData->srcRect.x + writeData->srcRect.width) / texSize.x,
        (float)writeData->srcRect.y / texSize.y,
        writeData->alpha,

        (1.0f - writeData->origin.x) * writeData->scale.x,
        (1.0f - writeData->origin.y) * writeData->scale.y,
        writeData->pos.x,
        writeData->pos.y,
        writeData->srcRect.width,
        writeData->srcRect.height,
        writeData->rot,
        texUnit,
        (float)(writeData->srcRect.x + writeData->srcRect.width) / texSize.x,
        (float)(writeData->srcRect.y + writeData->srcRect.height) / texSize.y,
        writeData->alpha,

        (0.0f - writeData->origin.x) * writeData->scale.x,
        (1.0f - writeData->origin.y) * writeData->scale.y,
        writeData->pos.x,
        writeData->pos.y,
        writeData->srcRect.width,
        writeData->srcRect.height,
        writeData->rot,
        texUnit,
        (float)writeData->srcRect.x / texSize.x,
        (float)(writeData->srcRect.y + writeData->srcRect.height) / texSize.y,
        writeData->alpha
    };

    const ZF3SpriteBatchGLIDs* const batchA = &i_renderer.spriteBatchGLIDs[batchIndex];
    glBindVertexArray(batchA->vertArrayGLID);
    glBindBuffer(GL_ARRAY_BUFFER, batchA->vertBufGLID);
    glBufferSubData(GL_ARRAY_BUFFER, slotIndex * sizeof(float) * ZF3_SPRITE_BATCH_SLOT_VERT_CNT, sizeof(verts), verts);

    ++batchTransData->slotsUsed;
}
