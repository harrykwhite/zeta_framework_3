#include <zf3_assets.h>

namespace zf3 {
    static bool load_textures(Textures* const textures, FILE* const fs) {
        const int pxDataBufSize = gk_texChannelCnt * gk_texSizeLimit.x * gk_texSizeLimit.y;
        const auto pxDataBuf = static_cast<unsigned char*>(malloc(pxDataBufSize)); // For temporarily storing the pixel data of each texture.

        if (!pxDataBuf) {
            return false;
        }

        fread(&textures->cnt, sizeof(textures->cnt), 1, fs);

        if (textures->cnt > 0) {
            glGenTextures(textures->cnt, textures->glIDs);

            for (int i = 0; i < textures->cnt; ++i) {
                fread(&textures->sizes[i], sizeof(textures->sizes[i]), 1, fs);

                fread(pxDataBuf, gk_texChannelCnt * textures->sizes[i].x * textures->sizes[i].y, 1, fs);

                glBindTexture(GL_TEXTURE_2D, textures->glIDs[i]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures->sizes[i].x, textures->sizes[i].y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pxDataBuf);
            }

            free(pxDataBuf);
        }

        return true;
    }

    static bool load_fonts(Fonts* const fonts, FILE* const fs) {
        const int pxDataBufSize = gk_texChannelCnt * gk_texSizeLimit.x * gk_texSizeLimit.y;
        const auto pxDataBuf = static_cast<unsigned char*>(malloc(pxDataBufSize)); // For temporarily storing the pixel data of each font texture.

        if (!pxDataBuf) {
            return false;
        }

        fread(&fonts->cnt, sizeof(fonts->cnt), 1, fs);

        if (fonts->cnt > 0) {
            glGenTextures(fonts->cnt, fonts->texGLIDs);

            for (int i = 0; i < fonts->cnt; ++i) {
                fread(&fonts->arrangementInfos[i], sizeof(fonts->arrangementInfos[i]), 1, fs);
                fread(&fonts->texSizes[i], sizeof(fonts->texSizes[i]), 1, fs);
                fread(pxDataBuf, gk_texPxDataSizeLimit, 1, fs);

                glBindTexture(GL_TEXTURE_2D, fonts->texGLIDs[i]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fonts->texSizes[i].x, fonts->texSizes[i].y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pxDataBuf);
            }

            free(pxDataBuf);
        }

        return true;
    }

    bool init_assets(Assets* const assets) {
        FILE* const fs = fopen(gk_assetsFileName, "rb");

        if (!fs) {
            return false;
        }

        load_textures(&assets->textures, fs);
        load_fonts(&assets->fonts, fs);

        fclose(fs);

        return true;
    }

    void clean_assets(Assets* const assets) {
        if (assets->fonts.cnt > 0) {
            glDeleteTextures(assets->fonts.cnt, assets->fonts.texGLIDs);
        }

        if (assets->textures.cnt > 0) {
            glDeleteTextures(assets->textures.cnt, assets->textures.glIDs);
        }
    }
}
