#include <zf3_rendering.h>

namespace zf3 {
    static constexpr int ik_spriteBatchSlotVertCnt = gk_spriteQuadShaderProgVertCnt * 4;
    static constexpr int ik_spriteBatchSlotVertsSize = sizeof(float) * ik_spriteBatchSlotVertCnt;

    static constexpr int ik_charBatchSlotVertsCnt = gk_charQuadShaderProgVertCnt * 4;
    static constexpr int ik_charBatchSlotVertsSize = sizeof(float) * ik_charBatchSlotVertsCnt;

    static QuadBuf gen_quad_buf(const int quadCnt, const bool sprite) {
        assert(quadCnt > 0);

        QuadBuf buf = {};

        const int vertCnt = sprite ? gk_spriteQuadShaderProgVertCnt : gk_charQuadShaderProgVertCnt;

        // Generate vertex array.
        glGenVertexArrays(1, &buf.vertArrayGLID);
        glBindVertexArray(buf.vertArrayGLID);

        // Generate vertex buffer.
        glGenBuffers(1, &buf.vertBufGLID);
        glBindBuffer(GL_ARRAY_BUFFER, buf.vertBufGLID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertCnt * 4 * quadCnt, nullptr, GL_DYNAMIC_DRAW);

        // Generate element buffer.
        {
            static constexpr int lk_quadLimit = max(gk_spriteBatchSlotLimit, gk_charBatchSlotLimit);
            static constexpr int lk_quadIndicesLen = 6 * lk_quadLimit;
            static unsigned short l_quadIndices[lk_quadIndicesLen];
            static bool l_quadIndicesInitialized = false;

            if (!l_quadIndicesInitialized) {
                for (int i = 0; i < lk_quadLimit; i++) {
                    l_quadIndices[(i * 6) + 0] = (i * 4) + 0;
                    l_quadIndices[(i * 6) + 1] = (i * 4) + 1;
                    l_quadIndices[(i * 6) + 2] = (i * 4) + 2;
                    l_quadIndices[(i * 6) + 3] = (i * 4) + 2;
                    l_quadIndices[(i * 6) + 4] = (i * 4) + 3;
                    l_quadIndices[(i * 6) + 5] = (i * 4) + 0;
                }

                l_quadIndicesInitialized = true;
            }

            glGenBuffers(1, &buf.elemBufGLID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.elemBufGLID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 6 * quadCnt, l_quadIndices, GL_STATIC_DRAW);
        }

        // Set vertex attribute pointers.
        const int vertsStride = sizeof(float) * vertCnt;

        if (sprite) {
            glVertexAttribPointer(0, 2, GL_FLOAT, false, vertsStride, reinterpret_cast<void*>(sizeof(float) * 0));
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, false, vertsStride, reinterpret_cast<void*>(sizeof(float) * 2));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 2, GL_FLOAT, false, vertsStride, reinterpret_cast<void*>(sizeof(float) * 4));
            glEnableVertexAttribArray(2);

            glVertexAttribPointer(3, 1, GL_FLOAT, false, vertsStride, reinterpret_cast<void*>(sizeof(float) * 6));
            glEnableVertexAttribArray(3);

            glVertexAttribPointer(4, 1, GL_FLOAT, false, vertsStride, reinterpret_cast<void*>(sizeof(float) * 7));
            glEnableVertexAttribArray(4);

            glVertexAttribPointer(5, 2, GL_FLOAT, false, vertsStride, reinterpret_cast<void*>(sizeof(float) * 8));
            glEnableVertexAttribArray(5);

            glVertexAttribPointer(6, 1, GL_FLOAT, false, vertsStride, reinterpret_cast<void*>(sizeof(float) * 10));
            glEnableVertexAttribArray(6);
        } else {
            glVertexAttribPointer(0, 2, GL_FLOAT, false, vertsStride, reinterpret_cast<void*>(sizeof(float) * 0));
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, false, vertsStride, reinterpret_cast<void*>(sizeof(float) * 2));
            glEnableVertexAttribArray(1);
        }

        // Unbind.
        glBindVertexArray(0);

        return buf;
    }

    static int add_tex_unit_to_sprite_batch(SpriteBatchTransData& batchTransData, const int texIndex) {
        for (int i = 0; i < batchTransData.texUnitsInUse; ++i) {
            if (batchTransData.texUnitTexIDs[i] == texIndex) {
                return i;
            }
        }

        if (batchTransData.texUnitsInUse == gk_texUnitLimit) {
            return -1;
        }

        batchTransData.texUnitTexIDs[batchTransData.texUnitsInUse] = texIndex;

        return batchTransData.texUnitsInUse++;
    }

    static Matrix4x4 create_cam_view_matrix(const Camera& cam) {
        Matrix4x4 mat = {};
        mat[0][0] = cam.scale;
        mat[1][1] = cam.scale;
        mat[3][3] = 1.0f;
        mat[3][0] = (-cam.pos.x * cam.scale) + (get_window_size().x / 2.0f);
        mat[3][1] = (-cam.pos.y * cam.scale) + (get_window_size().y / 2.0f);
        return mat;
    }

    void clean_renderer(Renderer& renderer) {
        for (int i = 0; i < renderer.layerCnt; ++i) {
            RenderLayer& layer = renderer.layers[i];

            for (int j = 0; j < layer.spriteBatchCnt; ++j) {
                glDeleteVertexArrays(1, &layer.spriteBatchQuadBufs[j].vertArrayGLID);
                glDeleteBuffers(1, &layer.spriteBatchQuadBufs[j].vertBufGLID);
                glDeleteBuffers(1, &layer.spriteBatchQuadBufs[j].elemBufGLID);
            }
        }

        zero_out(renderer);
    }

    void reset_renderer(Renderer& renderer, const int layerCnt, const int camLayerCnt, const Vec3D bgColor, const Vec2D camPos, const float camScale) {
        clean_renderer(renderer);
        renderer.layerCnt = layerCnt;
        renderer.camLayerCnt = camLayerCnt;
        renderer.bgColor = bgColor;
        renderer.cam.pos = camPos;
        renderer.cam.scale = camScale;
    }

    void render_all(const Renderer& renderer, const ShaderProgs& shaderProgs) {
        glClearColor(renderer.bgColor.r, renderer.bgColor.g, renderer.bgColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        static int lk_texUnits[gk_texUnitLimit];
        static bool lk_texUnitsInitialized = false;

        if (!lk_texUnitsInitialized) {
            for (int i = 0; i < gk_texUnitLimit; ++i) {
                lk_texUnits[i] = i;
            }

            lk_texUnitsInitialized = true;
        }

        const Matrix4x4 projMat = create_ortho_matrix_4x4(0.0f, get_window_size().x, get_window_size().y, 0.0f, -1.0f, 1.0f);
        const Matrix4x4 camViewMat = create_cam_view_matrix(renderer.cam);
        const Matrix4x4 defaultViewMat = create_identity_matrix_4x4();

        for (int i = 0; i < renderer.layerCnt; ++i) {
            //
            // Sprite Batches
            //
            glUseProgram(shaderProgs.spriteQuad.glID);

            glUniformMatrix4fv(shaderProgs.spriteQuad.projUniLoc, 1, false, reinterpret_cast<const float*>(projMat.elems));

            const Matrix4x4* const viewMat = i < renderer.camLayerCnt ? &camViewMat : &defaultViewMat;
            glUniformMatrix4fv(shaderProgs.spriteQuad.viewUniLoc, 1, false, reinterpret_cast<const float*>(viewMat->elems));

            glUniform1iv(shaderProgs.spriteQuad.texturesUniLoc, gk_texUnitLimit, lk_texUnits);

            const RenderLayer& layer = renderer.layers[i];

            for (int j = 0; j < layer.spriteBatchCnt; ++j) {
                const QuadBuf* const batchQuadBuf = &layer.spriteBatchQuadBufs[j];
                const SpriteBatchTransData* const batchTransData = &layer.spriteBatchTransDatas[j];

                glBindVertexArray(batchQuadBuf->vertArrayGLID);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batchQuadBuf->elemBufGLID);

                for (int k = 0; k < batchTransData->texUnitsInUse; ++k) {
                    glActiveTexture(GL_TEXTURE0 + k);
                    glBindTexture(GL_TEXTURE_2D, get_assets().textures.glIDs[batchTransData->texUnitTexIDs[k]]);
                }

                glDrawElements(GL_TRIANGLES, 6 * batchTransData->slotsUsed, GL_UNSIGNED_SHORT, nullptr);
            }

            //
            // Character Batches
            //
            glUseProgram(shaderProgs.charQuad.glID);

            glUniformMatrix4fv(shaderProgs.charQuad.projUniLoc, 1, false, reinterpret_cast<const float*>(projMat.elems));
            glUniformMatrix4fv(shaderProgs.charQuad.viewUniLoc, 1, false, reinterpret_cast<const float*>(viewMat->elems));

            for (int j = 0; j < gk_renderLayerCharBatchLimit; ++j) {
                if (!is_bit_active(layer.charBatchActivity.bytes, j)) {
                    continue;
                }

                const CharBatch* const batch = &layer.charBatches[j];

                glUniform2fv(shaderProgs.charQuad.posUniLoc, 1, reinterpret_cast<const float*>(&batch->displayProps.pos));
                glUniform1f(shaderProgs.charQuad.rotUniLoc, batch->displayProps.rot);
                glUniform4fv(shaderProgs.charQuad.blendUniLoc, 1, reinterpret_cast<const float*>(&batch->displayProps.blend));

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, get_assets().fonts.texGLIDs[batch->displayProps.fontIndex]);

                // Draw the batch.
                glBindVertexArray(batch->quadBuf.vertArrayGLID);
                glDrawElements(GL_TRIANGLES, 6 * batch->slotCnt, GL_UNSIGNED_SHORT, nullptr);
            }
        }
    }

    void empty_sprite_batches(Renderer& renderer) {
        for (int i = 0; i < renderer.layerCnt; ++i) {
            RenderLayer& layer = renderer.layers[i];
            zero_out(layer.spriteBatchTransDatas);
            layer.spriteBatchesFilled = 0;
        }
    }

    void write_to_sprite_batch(Renderer& renderer, const int layerIndex, const int texIndex, const Vec2D pos, const Rect& srcRect, const Vec2D origin, const float rot, const Vec2D scale, const float alpha) {
        assert(layerIndex >= 0 && layerIndex < renderer.layerCnt);

        RenderLayer& layer = renderer.layers[layerIndex];

        if (layer.spriteBatchCnt == 0) {
            layer.spriteBatchQuadBufs[layer.spriteBatchCnt] = gen_quad_buf(gk_spriteBatchSlotLimit, true);
            ++layer.spriteBatchCnt;
        }

        const int batchIndex = layer.spriteBatchesFilled;
        SpriteBatchTransData& batchTransData = layer.spriteBatchTransDatas[batchIndex];

        int texUnit;

        if (batchTransData.slotsUsed == gk_spriteBatchSlotLimit || (texUnit = add_tex_unit_to_sprite_batch(batchTransData, texIndex)) == -1) {
            ++layer.spriteBatchesFilled;

            if (layer.spriteBatchesFilled == layer.spriteBatchCnt) {
                layer.spriteBatchQuadBufs[layer.spriteBatchCnt] = gen_quad_buf(gk_spriteBatchSlotLimit, true);
                ++layer.spriteBatchCnt;
            }

            write_to_sprite_batch(renderer, layerIndex, texIndex, pos, srcRect, origin, rot, scale, alpha);
            return;
        }

        const int slotIndex = batchTransData.slotsUsed;
        const Pt2D texSize = get_assets().textures.sizes[texIndex];

        const float verts[] = {
            (0.0f - origin.x) * scale.x,
            (0.0f - origin.y) * scale.y,
            pos.x,
            pos.y,
            static_cast<float>(srcRect.width), static_cast<float>(srcRect.height),
            rot,
            static_cast<float>(texUnit),
            static_cast<float>(srcRect.x) / texSize.x, static_cast<float>(srcRect.y) / texSize.y,
            alpha,

            (1.0f - origin.x) * scale.x,
            (0.0f - origin.y) * scale.y,
            pos.x,
            pos.y,
            static_cast<float>(srcRect.width), static_cast<float>(srcRect.height),
            rot,
            static_cast<float>(texUnit),
            static_cast<float>(srcRect.x + srcRect.width) / texSize.x,
            static_cast<float>(srcRect.y) / texSize.y,
            alpha,

            (1.0f - origin.x) * scale.x,
            (1.0f - origin.y) * scale.y,
            pos.x,
            pos.y,
            static_cast<float>(srcRect.width), static_cast<float>(srcRect.height),
            rot,
            static_cast<float>(texUnit),
            static_cast<float>(srcRect.x + srcRect.width) / texSize.x,
            static_cast<float>(srcRect.y + srcRect.height) / texSize.y,
            alpha,

            (0.0f - origin.x) * scale.x,
            (1.0f - origin.y) * scale.y,
            pos.x,
            pos.y,
            static_cast<float>(srcRect.width), static_cast<float>(srcRect.height),
            rot,
            static_cast<float>(texUnit),
            static_cast<float>(srcRect.x) / texSize.x,
            static_cast<float>(srcRect.y + srcRect.height) / texSize.y,
            alpha
        };

        const QuadBuf* const batchQuadBuf = &layer.spriteBatchQuadBufs[batchIndex];
        glBindVertexArray(batchQuadBuf->vertArrayGLID);
        glBindBuffer(GL_ARRAY_BUFFER, batchQuadBuf->vertBufGLID);
        glBufferSubData(GL_ARRAY_BUFFER, slotIndex * sizeof(float) * ik_spriteBatchSlotVertCnt, sizeof(verts), verts);

        ++batchTransData.slotsUsed;
    }

    CharBatchID activate_any_char_batch(Renderer& renderer, const int layerIndex, const int slotCnt, const int fontIndex, const Vec2D pos) {
        assert(layerIndex >= 0 && layerIndex < gk_renderLayerLimit);
        assert(slotCnt > 0);

        RenderLayer& layer = renderer.layers[layerIndex];

        const int batchIndex = get_first_inactive_bit_index(layer.charBatchActivity.bytes, gk_renderLayerCharBatchLimit);
        assert(batchIndex != -1);

        activate_bit(layer.charBatchActivity.bytes, batchIndex);

        layer.charBatches[batchIndex] = {
            .quadBuf = gen_quad_buf(slotCnt, false),
            .slotCnt = slotCnt,
        };

        layer.charBatches[batchIndex].displayProps = {
            .fontIndex = fontIndex,
            .pos = pos,
            .blend = {1.0f, 1.0f, 1.0f, 1.0f}
        };

        return {
            .layerIndex = layerIndex,
            .batchIndex = batchIndex
        };
    }

    void deactivate_char_batch(Renderer& renderer, const CharBatchID id) {
        RenderLayer& layer = renderer.layers[id.layerIndex];
        deactivate_bit(layer.charBatchActivity.bytes, id.batchIndex);
    }

    void write_to_char_batch(Renderer& renderer, const CharBatchID id, const char* const text, const FontHorAlign horAlign, const FontVerAlign verAlign) {
        RenderLayer& layer = renderer.layers[id.layerIndex];
        CharBatch& batch = layer.charBatches[id.batchIndex];

        const int textLen = strlen(text);
        assert(textLen > 0 && textLen <= batch.slotCnt);

        const FontArrangementInfo& fontArrangementInfo = get_assets().fonts.arrangementInfos[batch.displayProps.fontIndex];
        const Pt2D fontTexSize = get_assets().fonts.texSizes[batch.displayProps.fontIndex];

        Vec2D charDrawPositions[gk_charBatchSlotLimit];
        Vec2D charDrawPosPen = {};

        int textLineWidths[gk_charBatchSlotLimit + 1];
        int textFirstLineMinOffs = 0;
        bool textFirstLineMinOffsUpdated = false;
        int textLastLineMaxHeight = 0;
        bool textLastLineMaxHeightUpdated = false;
        int textLineCnter = 0;

        for (int i = 0; i < textLen; i++) {
            if (text[i] == '\n') {
                textLineWidths[textLineCnter] = charDrawPosPen.x;

                if (!textFirstLineMinOffsUpdated) {
                    // Set the first line minimum offset to the vertical offset of the space character.
                    textFirstLineMinOffs = fontArrangementInfo.chars.verOffsets[0];
                    textFirstLineMinOffsUpdated = true;
                }

                // Set the last line maximum height to the height of a space.
                textLastLineMaxHeight = fontArrangementInfo.chars.verOffsets[0] + fontArrangementInfo.chars.srcRects[0].height;

                textLastLineMaxHeightUpdated = false;

                textLineCnter++;

                // Move the pen to a new line.
                charDrawPosPen.x = 0.0f;
                charDrawPosPen.y += fontArrangementInfo.lineHeight;

                continue;
            }

            const int textCharIndex = text[i] - gk_fontCharRangeBegin;

            // If we are on the first line, update the first line minimum offset.
            if (textLineCnter == 0) {
                if (!textFirstLineMinOffsUpdated) {
                    textFirstLineMinOffs = fontArrangementInfo.chars.verOffsets[textCharIndex];
                    textFirstLineMinOffsUpdated = true;
                } else {
                    textFirstLineMinOffs = min(fontArrangementInfo.chars.verOffsets[textCharIndex], textFirstLineMinOffs);
                }
            }

            if (!textLastLineMaxHeightUpdated) {
                textLastLineMaxHeight = fontArrangementInfo.chars.verOffsets[textCharIndex] + fontArrangementInfo.chars.srcRects[textCharIndex].height;
                textLastLineMaxHeightUpdated = true;
            } else {
                textLastLineMaxHeight = max(fontArrangementInfo.chars.verOffsets[textCharIndex] + fontArrangementInfo.chars.srcRects[textCharIndex].height, textLastLineMaxHeight);
            }

            if (i > 0) {
                // Apply kerning based on the previous character.
                const int textCharIndexLast = text[i - 1] - gk_fontCharRangeBegin;
                charDrawPosPen.x += fontArrangementInfo.chars.kernings[(textCharIndex * gk_fontCharRangeSize) + textCharIndexLast];
            }

            charDrawPositions[i].x = charDrawPosPen.x + fontArrangementInfo.chars.horOffsets[textCharIndex];
            charDrawPositions[i].y = charDrawPosPen.y + fontArrangementInfo.chars.verOffsets[textCharIndex];

            charDrawPosPen.x += fontArrangementInfo.chars.horAdvances[textCharIndex];
        }

        textLineWidths[textLineCnter] = charDrawPosPen.x;
        textLineCnter = 0;

        const int textHeight = textFirstLineMinOffs + charDrawPosPen.y + textLastLineMaxHeight;

        // Clear the batch so it will have only the new characters.
        clear_char_batch(renderer, id);

        // Reserve memory to temporarily hold the vertex data for the characters, so we can submit it all at once at the end.
        const int vertsLen = ik_charBatchSlotVertsCnt * textLen;
        const auto verts = static_cast<float*>(calloc(vertsLen, sizeof(float)));

        if (!verts) {
            return;
        }

        // Write the vertex data.
        for (int i = 0; i < textLen; i++) {
            if (text[i] == '\n') {
                textLineCnter++;
                continue;
            }

            if (text[i] == ' ') {
                continue;
            }

            const int charIndex = text[i] - gk_fontCharRangeBegin;

            const Vec2D charDrawPos = {
                charDrawPositions[i].x - (textLineWidths[textLineCnter] * horAlign * 0.5f),
                charDrawPositions[i].y - (textHeight * verAlign * 0.5f)
            };

            const Vec2D charTexCoordsTopLeft = {
                static_cast<float>(fontArrangementInfo.chars.srcRects[charIndex].x) / fontTexSize.x,
                static_cast<float>(fontArrangementInfo.chars.srcRects[charIndex].y) / fontTexSize.y
            };

            const Vec2D charTexCoordsBottomRight = {
                static_cast<float>(rect_right(fontArrangementInfo.chars.srcRects[charIndex])) / fontTexSize.x,
                static_cast<float>(rect_bottom(fontArrangementInfo.chars.srcRects[charIndex])) / fontTexSize.y
            };

            float* const slotVerts = verts + (i * ik_charBatchSlotVertsCnt);

            slotVerts[0] = charDrawPos.x;
            slotVerts[1] = charDrawPos.y;
            slotVerts[2] = charTexCoordsTopLeft.x;
            slotVerts[3] = charTexCoordsTopLeft.y;

            slotVerts[4] = charDrawPos.x + fontArrangementInfo.chars.srcRects[charIndex].width;
            slotVerts[5] = charDrawPos.y;
            slotVerts[6] = charTexCoordsBottomRight.x;
            slotVerts[7] = charTexCoordsTopLeft.y;

            slotVerts[8] = charDrawPos.x + fontArrangementInfo.chars.srcRects[charIndex].width;
            slotVerts[9] = charDrawPos.y + fontArrangementInfo.chars.srcRects[charIndex].height;
            slotVerts[10] = charTexCoordsBottomRight.x;
            slotVerts[11] = charTexCoordsBottomRight.y;

            slotVerts[12] = charDrawPos.x;
            slotVerts[13] = charDrawPos.y + fontArrangementInfo.chars.srcRects[charIndex].height;
            slotVerts[14] = charTexCoordsTopLeft.x;
            slotVerts[15] = charTexCoordsBottomRight.y;
        }

        // Submit the vertex data.
        glBindVertexArray(batch.quadBuf.vertArrayGLID);
        glBindBuffer(GL_ARRAY_BUFFER, batch.quadBuf.vertBufGLID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts[0]) * vertsLen, verts);

        free(verts);
    }

    void clear_char_batch(const Renderer& renderer, const CharBatchID id) {
        const RenderLayer& layer = renderer.layers[id.layerIndex];
        const CharBatch& batch = layer.charBatches[id.batchIndex];

        glBindVertexArray(batch.quadBuf.vertArrayGLID);
        glBindBuffer(GL_ARRAY_BUFFER, batch.quadBuf.vertBufGLID);
        glBufferData(GL_ARRAY_BUFFER, ik_charBatchSlotVertsSize * batch.slotCnt, nullptr, GL_DYNAMIC_DRAW);
    }
}
