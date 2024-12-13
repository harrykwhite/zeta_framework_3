#include "zf3ap.h"

bool pack_textures(cJSON* const instrsCJ, FILE* const outputFS, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen) {
    // Get the textures array.
    const cJSON* const cJSONTextures = getCJSONAssetsArrayAndWriteAssetCnt(instrsCJ, outputFS, "textures");

    if (!cJSONTextures) {
        return true;
    }

    // Iterate through the textures array.
    const cJSON* cJSONTex = NULL;

    cJSON_ArrayForEach(cJSONTex, cJSONTextures) {
        if (!cJSON_IsString(cJSONTex)) {
            return false;
        }

        // Get the texture file path.
        strncpy(srcAssetFilePathBuf + srcAssetFilePathStartLen, cJSONTex->valuestring, gk_srcAssetFilePathBufSize - srcAssetFilePathStartLen);

        if (srcAssetFilePathBuf[gk_srcAssetFilePathBufSize - 1]) {
            zf3::log_error("The texture relative file path of \"%s\" exceeds the size limit of %d characters!", cJSONTex->valuestring, gk_srcAssetFilePathBufSize - 1 - srcAssetFilePathStartLen);
            return false;
        }

        // Load the texture data.
        zf3::Vec2DInt texSize;
        stbi_uc* const texPxData = stbi_load(srcAssetFilePathBuf, &texSize.x, &texSize.y, NULL, zf3::gk_texChannelCnt);

        if (!texPxData) {
            zf3::log_error("Failed to load pixel data for texture with relative file path \"%s\"!", cJSONTex->valuestring);
            return false;
        }

        // Write the texture size and pixel data to the output file.
        fwrite(&texSize, sizeof(texSize), 1, outputFS);
        fwrite(texPxData, sizeof(*texPxData), texSize.x * texSize.y * zf3::gk_texChannelCnt, outputFS);

        zf3::log("Packed texture with file path \"%s\".", srcAssetFilePathBuf);

        stbi_image_free(texPxData);
    }

    return true;
}
