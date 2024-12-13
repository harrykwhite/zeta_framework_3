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
        strncpy(srcAssetFilePathBuf + srcAssetFilePathStartLen, cJSONTex->valuestring, SRC_ASSET_FILE_PATH_BUF_SIZE - srcAssetFilePathStartLen);

        if (srcAssetFilePathBuf[SRC_ASSET_FILE_PATH_BUF_SIZE - 1]) {
            zf3_log_error("The texture relative file path of \"%s\" exceeds the size limit of %d characters!", cJSONTex->valuestring, SRC_ASSET_FILE_PATH_BUF_SIZE - 1 - srcAssetFilePathStartLen);
            return false;
        }

        // Load the texture data.
        ZF3Vec2DInt texSize;
        stbi_uc* const texPxData = stbi_load(srcAssetFilePathBuf, &texSize.x, &texSize.y, NULL, ZF3_TEX_CHANNEL_COUNT);

        if (!texPxData) {
            zf3_log_error("Failed to load pixel data for texture with relative file path \"%s\"!", cJSONTex->valuestring);
            return false;
        }

        // Write the texture size and pixel data to the output file.
        fwrite(&texSize, sizeof(texSize), 1, outputFS);
        fwrite(texPxData, sizeof(*texPxData), texSize.x * texSize.y * ZF3_TEX_CHANNEL_COUNT, outputFS);

        zf3_log("Packed texture with file path \"%s\".", srcAssetFilePathBuf);

        stbi_image_free(texPxData);
    }

    return true;
}
