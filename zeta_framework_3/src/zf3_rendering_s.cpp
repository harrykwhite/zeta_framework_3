#include <zf3_rendering.h>

namespace zf3 {

GLID i_spriteQuadShaderProgGLID;
int i_spriteQuadShaderProgProjUniLoc;
int i_spriteQuadShaderProgViewUniLoc;
int i_spriteQuadShaderProgTexturesUniLoc;

static const char* i_spriteQuadVertShaderSrc =
"#version 430 core\n"
"layout (location = 0) in vec2 a_vert;\n"
"layout (location = 1) in vec2 a_pos;\n"
"layout (location = 2) in vec2 a_size;\n"
"layout (location = 3) in float a_rot;\n"
"layout (location = 4) in float a_texIndex;\n"
"layout (location = 5) in vec2 a_texCoord;\n"
"layout (location = 6) in float a_alpha;\n"
"\n"
"out flat int v_texIndex;\n"
"out vec2 v_texCoord;\n"
"out float v_alpha;\n"
"\n"
"uniform mat4 u_view;\n"
"uniform mat4 u_proj;\n"
"\n"
"void main()\n"
"{\n"
"    float rotCos = cos(a_rot);\n"
"    float rotSin = -sin(a_rot);\n"
"\n"
"    mat4 model = mat4(\n"
"        vec4(a_size.x * rotCos, a_size.x * rotSin, 0.0f, 0.0f),\n"
"        vec4(a_size.y * -rotSin, a_size.y * rotCos, 0.0f, 0.0f),\n"
"        vec4(0.0f, 0.0f, 1.0f, 0.0f),\n"
"        vec4(a_pos.x, a_pos.y, 0.0f, 1.0f)\n"
"    );\n"
"\n"
"    gl_Position = u_proj * u_view * model * vec4(a_vert, 0.0f, 1.0f);\n"
"\n"
"    v_texIndex = int(a_texIndex);\n"
"    v_texCoord = a_texCoord;\n"
"    v_alpha = a_alpha;\n"
"}\n";

static const char* ik_spriteQuadFragShaderSrc =
"#version 430 core\n"
"\n"
"in flat int v_texIndex;\n"
"in vec2 v_texCoord;\n"
"in float v_alpha;\n"
"\n"
"out vec4 o_fragColor;\n"
"\n"
"uniform sampler2D u_textures[32];\n"
"\n"
"void main()\n"
"{\n"
"    vec4 texColor = texture(u_textures[v_texIndex], v_texCoord);\n"
"    o_fragColor = texColor * vec4(1.0f, 1.0f, 1.0f, v_alpha);\n"
"}\n";

static constexpr int ik_quadLimit = gk_spriteBatchSlotLimit;
static constexpr int ik_quadIndicesLen = 6 * ik_quadLimit;

static unsigned short i_quadIndices[ik_quadIndicesLen];
static int i_texUnits[gk_texUnitLimit];

RenderLayer i_layers[gk_renderLayerLimit];
int i_layerCnt;
int i_camLayerCnt; // Layers 0 through to this number are drawn with a camera view matrix.

Color g_bgColor = {0.0f, 0.0f, 0.0f, 1.0f};

Camera g_camera = {
    .scale = 1.0f
};

static void clean_render_layers() {
    for (int i = 0; i < i_layerCnt; ++i) {
        RenderLayer* const layer = &i_layers[i];

        for (int j = 0; j < layer->spriteBatchCnt; ++j) {
            glDeleteVertexArrays(1, &layer->spriteBatchGLIDs[j].vertArrayGLID);
            glDeleteBuffers(1, &layer->spriteBatchGLIDs[j].vertBufGLID);
            glDeleteBuffers(1, &layer->spriteBatchGLIDs[j].elemBufGLID);
        }
    }

    memset(i_layers, 0, sizeof(*i_layers) * i_layerCnt);

    i_layerCnt = 0;
    i_camLayerCnt = 0;
}

static void add_sprite_batch(const int layerIndex) {
    assert(layerIndex >= 0 && layerIndex < i_layerCnt);

    RenderLayer* const layer = &i_layers[layerIndex];
    const int batchIndex = layer->spriteBatchCnt;

    SpriteBatchGLIDs* const glIDs = &layer->spriteBatchGLIDs[batchIndex];

    // Generate vertex array.
    glGenVertexArrays(1, &glIDs->vertArrayGLID);
    glBindVertexArray(glIDs->vertArrayGLID);

    // Generate vertex buffer.
    glGenBuffers(1, &glIDs->vertBufGLID);
    glBindBuffer(GL_ARRAY_BUFFER, glIDs->vertBufGLID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * gk_spriteQuadShaderProgVertCnt * 4 * gk_spriteBatchSlotLimit, nullptr, GL_DYNAMIC_DRAW);

    // Generate element buffer.
    glGenBuffers(1, &glIDs->elemBufGLID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIDs->elemBufGLID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 6 * gk_spriteBatchSlotLimit, i_quadIndices, GL_STATIC_DRAW);

    // Set vertex attribute pointers.
    const int vertsStride = sizeof(float) * gk_spriteQuadShaderProgVertCnt;

    glVertexAttribPointer(0, 2, GL_FLOAT, false, vertsStride, reinterpret_cast<const void*>(sizeof(float) * 0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, false, vertsStride, reinterpret_cast<const void*>(sizeof(float) * 2));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, false, vertsStride, reinterpret_cast<const void*>(sizeof(float) * 4));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, false, vertsStride, reinterpret_cast<const void*>(sizeof(float) * 6));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 1, GL_FLOAT, false, vertsStride, reinterpret_cast<const void*>(sizeof(float) * 7));
    glEnableVertexAttribArray(4);

    glVertexAttribPointer(5, 2, GL_FLOAT, false, vertsStride, reinterpret_cast<const void*>(sizeof(float) * 8));
    glEnableVertexAttribArray(5);

    glVertexAttribPointer(6, 1, GL_FLOAT, false, vertsStride, reinterpret_cast<const void*>(sizeof(float) * 10));
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

static Matrix4x4 create_cam_view_matrix() {
    Matrix4x4 mat = {};
    mat[0][0] = g_camera.scale;
    mat[1][1] = g_camera.scale;
    mat[3][3] = 1.0f;
    mat[3][0] = (-g_camera.pos.x * g_camera.scale) + (get_window_size().x / 2.0f);
    mat[3][1] = (-g_camera.pos.y * g_camera.scale) + (get_window_size().y / 2.0f);
    return mat;
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

    // Set up sprite quad shader program.
    i_spriteQuadShaderProgGLID = create_shader_prog_from_srcs(i_spriteQuadVertShaderSrc, ik_spriteQuadFragShaderSrc);
    assert(i_spriteQuadShaderProgGLID);

    i_spriteQuadShaderProgProjUniLoc = glGetUniformLocation(i_spriteQuadShaderProgGLID, "u_proj");
    i_spriteQuadShaderProgViewUniLoc = glGetUniformLocation(i_spriteQuadShaderProgGLID, "u_view");
    i_spriteQuadShaderProgTexturesUniLoc = glGetUniformLocation(i_spriteQuadShaderProgGLID, "u_textures");
}

void rendering_cleanup() {
    clean_render_layers();

    glDeleteProgram(i_spriteQuadShaderProgGLID);
    i_spriteQuadShaderProgGLID = 0;
}

void load_render_layers(const int layerCnt, const int camLayerCnt) {
    assert(layerCnt > 0 && layerCnt <= gk_renderLayerLimit);
    assert(camLayerCnt >= 0 && camLayerCnt <= layerCnt);

    clean_render_layers();
    i_layerCnt = layerCnt;
    i_camLayerCnt = camLayerCnt;
}

void render_all() {
    assert(g_bgColor.a == 1.0f);
    assert(g_camera.scale >= 1.0f);

    glClearColor(g_bgColor.r, g_bgColor.g, g_bgColor.b, g_bgColor.a);
    glClear(GL_COLOR_BUFFER_BIT);

    const Matrix4x4 projMat = create_ortho_matrix_4x4(0.0f, get_window_size().x, get_window_size().y, 0.0f, -1.0f, 1.0f);
    const Matrix4x4 camViewMat = create_cam_view_matrix();
    const Matrix4x4 defaultViewMat = create_identity_matrix_4x4();

    for (int i = 0; i < i_layerCnt; ++i) {
        glUseProgram(i_spriteQuadShaderProgGLID);

        glUniformMatrix4fv(i_spriteQuadShaderProgProjUniLoc, 1, false, (const float*)projMat.elems);

        const Matrix4x4* const viewMat = i < i_camLayerCnt ? &camViewMat : &defaultViewMat; // TODO: Pull this check out of the loop.
        glUniformMatrix4fv(i_spriteQuadShaderProgViewUniLoc, 1, false, (const float*)viewMat->elems);

        glUniform1iv(i_spriteQuadShaderProgTexturesUniLoc, gk_texUnitLimit, i_texUnits);

        const RenderLayer* const layer = &i_layers[i];

        for (int j = 0; j < layer->spriteBatchCnt; ++j) {
            const SpriteBatchGLIDs* const batchGLIDs = &layer->spriteBatchGLIDs[j];
            const SpriteBatchTransData* const batchTransData = &layer->spriteBatchTransDatas[j];

            glBindVertexArray(batchGLIDs->vertArrayGLID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batchGLIDs->elemBufGLID);

            for (int k = 0; k < batchTransData->texUnitsInUse; ++k) {
                glActiveTexture(GL_TEXTURE0 + i_texUnits[k]);
                glBindTexture(GL_TEXTURE_2D, get_assets().texGLIDs[batchTransData->texUnitTexIDs[k]]);
            }

            glDrawElements(GL_TRIANGLES, 6 * batchTransData->slotsUsed, GL_UNSIGNED_SHORT, nullptr);
        }
    }
}

void empty_sprite_batches() {
    for (int i = 0; i < i_layerCnt; ++i) {
        RenderLayer* const layer = &i_layers[i];
        memset(layer->spriteBatchTransDatas, 0, sizeof(layer->spriteBatchTransDatas));
        layer->spriteBatchesFilled = 0;
    }
}

void write_to_sprite_batch(const int layerIndex, const int texIndex, const Vec2D pos, const Rect srcRect, const Vec2D origin, const float rot, const Vec2D scale, const float alpha) {
    assert(layerIndex >= 0 && layerIndex < i_layerCnt);

    RenderLayer* const layer = &i_layers[layerIndex];

    if (layer->spriteBatchCnt == 0) {
        add_sprite_batch(layerIndex);
    }

    const int batchIndex = layer->spriteBatchesFilled;
    SpriteBatchTransData* const batchTransData = &layer->spriteBatchTransDatas[batchIndex];

    int texUnit;

    if (batchTransData->slotsUsed == gk_spriteBatchSlotLimit || (texUnit = add_tex_unit_to_sprite_batch(batchTransData, texIndex)) == -1) {
        ++layer->spriteBatchesFilled;

        if (layer->spriteBatchesFilled == layer->spriteBatchCnt) {
            add_sprite_batch(layerIndex);
        }

        write_to_sprite_batch(layerIndex, texIndex, pos, srcRect, origin, rot, scale, alpha);

        return;
    }

    const int slotIndex = batchTransData->slotsUsed;

    const Vec2DInt texSize = get_assets().texSizes[texIndex];

    const float verts[] = {
        (0.0f - origin.x) * scale.x,
        (0.0f - origin.y) * scale.y,
        pos.x,
        pos.y,
        static_cast<float>(srcRect.width),
        static_cast<float>(srcRect.height),
        rot,
        static_cast<float>(texUnit),
        static_cast<float>(srcRect.x) / texSize.x,
        static_cast<float>(srcRect.y) / texSize.y,
        alpha,

        (1.0f - origin.x) * scale.x,
        (0.0f - origin.y) * scale.y,
        pos.x,
        pos.y,
        static_cast<float>(srcRect.width),
        static_cast<float>(srcRect.height),
        rot,
        static_cast<float>(texUnit),
        static_cast<float>(srcRect.x + srcRect.width) / texSize.x,
        static_cast<float>(srcRect.y) / texSize.y,
        alpha,

        (1.0f - origin.x) * scale.x,
        (1.0f - origin.y) * scale.y,
        pos.x,
        pos.y,
        static_cast<float>(srcRect.width),
        static_cast<float>(srcRect.height),
        rot,
        static_cast<float>(texUnit),
        static_cast<float>(srcRect.x + srcRect.width) / texSize.x,
        static_cast<float>(srcRect.y + srcRect.height) / texSize.y,
        alpha,

        (0.0f - origin.x) * scale.x,
        (1.0f - origin.y) * scale.y,
        pos.x,
        pos.y,
        static_cast<float>(srcRect.width),
        static_cast<float>(srcRect.height),
        rot,
        static_cast<float>(texUnit),
        static_cast<float>(srcRect.x) / texSize.x,
        static_cast<float>(srcRect.y + srcRect.height) / texSize.y,
        alpha
    };

    const SpriteBatchGLIDs* const batchGLIDs = &layer->spriteBatchGLIDs[batchIndex];
    glBindVertexArray(batchGLIDs->vertArrayGLID);
    glBindBuffer(GL_ARRAY_BUFFER, batchGLIDs->vertBufGLID);
    glBufferSubData(GL_ARRAY_BUFFER, slotIndex * sizeof(float) * gk_spriteBatchSlotVertCnt, sizeof(verts), verts);

    ++batchTransData->slotsUsed;
}

}
