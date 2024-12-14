#include <zf3_assets.h>

namespace zf3 {

static Assets* i_assets;

static bool load_textures(FILE* const fs) {
    if (i_assets->texCnt > 0) {
        const int pxDataBufSize = gk_texChannelCnt * gk_texSizeLimit.x * gk_texSizeLimit.y;
        unsigned char* const pxDataBuf = static_cast<unsigned char*>(malloc(pxDataBufSize)); // For temporarily storing the pixel data of each texture.

        if (!pxDataBuf) {
            return false;
        }

        glGenTextures(i_assets->texCnt, i_assets->texGLIDs);

        for (int i = 0; i < i_assets->texCnt; ++i) {
            fread(&i_assets->texSizes[i], sizeof(i_assets->texSizes[i]), 1, fs);

            fread(pxDataBuf, gk_texChannelCnt * i_assets->texSizes[i].x * i_assets->texSizes[i].y, 1, fs);

            glBindTexture(GL_TEXTURE_2D, i_assets->texGLIDs[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i_assets->texSizes[i].x, i_assets->texSizes[i].y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pxDataBuf);
        }

        free(pxDataBuf);
    }

    return true;
}

bool load_assets() {
    assert(!i_assets);

    FILE* const fs = fopen(gk_assetsFileName, "rb");

    if (!fs) {
        return false;
    }

    i_assets = static_cast<Assets*>(calloc(1, sizeof(*i_assets)));

    if (!i_assets) {
        return false;
    }

    fread(&i_assets->texCnt, sizeof(i_assets->texCnt), 1, fs);

    const bool loadSucceeded = load_textures(fs);

    fclose(fs);

    return loadSucceeded;
}

void unload_assets() {
    if (!i_assets) {
        return;
    }

    if (i_assets->texCnt > 0) {
        glDeleteTextures(i_assets->texCnt, i_assets->texGLIDs);
    }

    free(i_assets);
    i_assets = nullptr;
}

const Assets& get_assets() {
    return *i_assets;
}

}
