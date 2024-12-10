#include <zf3.h>

void zf3_sprite_renderer_cleanup(ZF3SpriteRenderer* const renderer) {
    glDeleteVertexArrays(renderer->numSpritesGenerated, renderer->spriteVertArrayGLIDs);
    glDeleteBuffers(renderer->numSpritesGenerated, renderer->spriteVertBufGLIDs);
    glDeleteBuffers(renderer->numSpritesGenerated, renderer->spriteElemBufGLIDs);

    memset(renderer, 0, sizeof(*renderer));
}

ZF3Sprite* zf3_gen_sprites(ZF3SpriteRenderer* const renderer, const int spriteCnt) {
    const int spritesBeginIndex = renderer->numSpritesGenerated;
    const int spritesEndIndex = spritesBeginIndex + spriteCnt;

    assert(spritesEndIndex <= ZF3_SPRITE_LIMIT);

    glGenVertexArrays(spriteCnt, &renderer->spriteVertArrayGLIDs[spritesBeginIndex]);
    glGenBuffers(spriteCnt, &renderer->spriteVertBufGLIDs[spritesBeginIndex]);
    glGenBuffers(spriteCnt, &renderer->spriteElemBufGLIDs[spritesBeginIndex]);

    for (int i = spritesBeginIndex; i < spritesEndIndex; ++i) {
        glBindVertexArray(renderer->spriteVertArrayGLIDs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, renderer->spriteVertBufGLIDs[i]);

        const float verts[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->spriteElemBufGLIDs[i]);

        const unsigned short indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        const int vertsStride = sizeof(float) * 2;

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vertsStride, NULL);
        glEnableVertexAttribArray(0);
    }

    glBindVertexArray(0);

    renderer->numSpritesGenerated += spriteCnt;

    return &renderer->sprites[spritesBeginIndex];
}

void zf3_render_sprites(const ZF3SpriteRenderer* const renderer) {
    const ZF3ShaderProgs* const progs = zf3_get_shader_progs();

    glUseProgram(progs->spriteGLID);

    ZF3Matrix4x4 viewMat;
    zf3_init_identity_matrix_4x4(&viewMat);
    glUniformMatrix4fv(progs->spriteViewUniLoc, 1, GL_FALSE, (GLfloat*)&viewMat);

    ZF3Matrix4x4 projMat;
    zf3_init_ortho_matrix_4x4(&projMat, 0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(progs->spriteProjUniLoc, 1, GL_FALSE, (GLfloat*)&projMat);

    for (int i = 0; i < renderer->numSpritesGenerated; ++i) {
        const ZF3Sprite* const sprite = &renderer->sprites[i];

        glUniform2fv(progs->spritePosUniLoc, 1, (GLfloat*)&sprite->pos);
        glUniform2fv(progs->spriteSizeUniLoc, 1, (GLfloat*)&sprite->size);
        glUniform1f(progs->spriteRotUniLoc, sprite->rot);
        glUniform1f(progs->spriteAlphaUniLoc, sprite->alpha);

        glBindTexture(GL_TEXTURE_2D, zf3_get_assets()->texGLIDs[sprite->texIndex]);

        glBindVertexArray(renderer->spriteVertArrayGLIDs[i]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
    }
}
