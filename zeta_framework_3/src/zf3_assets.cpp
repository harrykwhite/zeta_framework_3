#include <zf3_assets.h>

namespace zf3 {

static bool load_textures(Assets* const assets, FILE* const fs) {
    if (assets->texCnt > 0) {
        const int pxDataBufSize = gk_texChannelCnt * gk_texSizeLimit.x * gk_texSizeLimit.y;
        unsigned char* const pxDataBuf = static_cast<unsigned char*>(malloc(pxDataBufSize)); // For temporarily storing the pixel data of each texture.

        if (!pxDataBuf) {
            return false;
        }

        glGenTextures(assets->texCnt, assets->texGLIDs);

        // Read the sizes and pixel data of textures and finish setting them up.
        for (int i = 0; i < assets->texCnt; ++i) {
            fread(&assets->texSizes[i], sizeof(assets->texSizes[i]), 1, fs);

            fread(pxDataBuf, gk_texChannelCnt * assets->texSizes[i].x * assets->texSizes[i].y, 1, fs);

            glBindTexture(GL_TEXTURE_2D, assets->texGLIDs[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, assets->texSizes[i].x, assets->texSizes[i].y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pxDataBuf);
        }

        free(pxDataBuf);
    }

    return true;
}

bool load_assets(Assets* const assets) {
    memset(assets, 0, sizeof(*assets));

    FILE* const fs = fopen(gk_assetsFileName, "rb");

    if (!fs) {
        return false;
    }

    fread(&assets->texCnt, sizeof(assets->texCnt), 1, fs);

    const bool loadSucceeded = load_textures(assets, fs);

    fclose(fs);

    return loadSucceeded;
}

void unload_assets(Assets* const assets) {
    glDeleteTextures(assets->texCnt, assets->texGLIDs);
    memset(assets, 0, sizeof(*assets));
}

}
